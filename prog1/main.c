#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stdbool.h>
#include <time.h>

#include "helpers.h"
#include "shared_region.h"

void dispatcher(char *files[], int nFiles, int size);
void worker();

int main(int argc, char *argv[])
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    // two or more processes is required
    if (size <= 1)
    {
        if (rank == 0)
            printf("Wrong number of processes! It must be greater than 1.\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    /* DISPATCHER PROCESS
        Read file
        Send a chunk with 52 of sizer to worker 
        Receive the results of worker
        Save on shared region
        Print the final results
    */
    if (rank == 0)
    {
        struct timespec start, finish;              /* time limits */
        clock_gettime(CLOCK_MONOTONIC_RAW, &start); /* begin of measurement */

        char *files[argc - 2];

        for (int i = 1; i < argc; i++)
            files[i - 1] = argv[i];

        dispatcher(files, argc - 1, size);              /*Start dispatcher*/

        clock_gettime(CLOCK_MONOTONIC_RAW, &finish); /* end of measurement */

        printf("\nElapsed tim = %.6f s\n", (finish.tv_sec - start.tv_sec) / 1.0 + (finish.tv_nsec - start.tv_nsec) / 1000000000.0);
    }
    /* WORKER PROCESS
        Receive the chunk
        Process the data
        Send the results
    */
    else
    {
        worker();
    }

    MPI_Finalize();
    return 0;
}

/**
 * @brief While has work to do, receive chunks, process them, and send to dispatcher the results
 * 
 */
void worker()
{
    while (1)
    {
        int is_in_word = 0;
        unsigned int last_c = 0;
        unsigned int consonant_counter = 0;
        unsigned int vowel_counter = 0;
        unsigned int words_counter = 0;
        bool  moreWork;
        unsigned int buffer[CHUNK_SIZE];

        MPI_Recv(&moreWork, 1, MPI_C_BOOL, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (!moreWork)
        {
            break;
        }


        MPI_Recv(buffer, CHUNK_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 0; i < CHUNK_SIZE; i++)
        {
            unsigned int c = buffer[i];

            if (c == 0)
                break;

            if (is_in_word && isSeparator(c))
            {
                if (isConsonant(last_c))
                {
                    consonant_counter++;
                }
                is_in_word = 0;
            }
            else if (!is_in_word && isVowel(c))
            {
                is_in_word = 1;
                words_counter++;
                vowel_counter++;
            }
            else if (!is_in_word && (isNumber(c) || isConsonant(c) || c == '_'))
            {
                is_in_word = 1;
                words_counter++;
            }

            if (isSeparator(c) || isVowel(c) || isNumber(c) || isConsonant(c) || c == '_' || c == '\'')
            {
                last_c = c;
            }
        }

        MPI_Send(&words_counter, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&vowel_counter, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&consonant_counter, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
   
    }
}

/**
 * @brief Read the files, get a chunks of text file, send the chunk to worker, receive the results of 
 *        worker, and save the results
 * 
 * @param files 
 * @param nFiles 
 * @param size 
 */
void dispatcher(char *files[], int nFiles, int size)
{

    fileInfo file_info[nFiles];

    loadFilesInfo(nFiles, files, file_info);

    bool moreWork = true;

    for (int file_id = 0; file_id < nFiles; file_id++) // Loop for files
    {

        char *file_path = files[file_id];
        FILE *file = fopen(file_path, "rb");
        if (file == NULL)                               // check th file
        {
            printf("\nUnable to open file.\n");
            exit(EXIT_FAILURE);
        }

        unsigned int c;
        bool file_finished = false;
        while (!file_finished)                          // while file not finished
        {
            int last_worker = 0;
            for (int workId = 1; workId < size; workId++)          // for all process worker
            {

                unsigned int last_separator = 0;
                long backward_bytes = 0;
                unsigned int buffer[CHUNK_SIZE];


                for (unsigned int i = 0; i < CHUNK_SIZE; i++)   // will create the buffer/chunker with 52
                {

                    c = read_u8char(file);
                    if (isSeparator(c))
                        last_separator = i;
                    else if (c == 0)
                        file_finished = true;
                    buffer[i] = c;
                }
                last_worker++;

                for (unsigned int i = last_separator + 1; i < CHUNK_SIZE; i++)  // remove all chars after last separator
                {
                    if (buffer[i] != 0)
                    {
                        backward_bytes += get_needed_bytes(buffer[i]);
                        buffer[i] = 0;
                    }
                }

                fseek(file, -backward_bytes, SEEK_CUR);                 // to go back on file
     
                MPI_Send(&moreWork, 1, MPI_C_BOOL, workId, 0, MPI_COMM_WORLD); // Send value to identify if will continue or not


                MPI_Send(buffer, CHUNK_SIZE, MPI_INT, workId, 0, MPI_COMM_WORLD);           // Send the chunker
                if (file_finished){
                    break;
                }
            }

            for (int workId = 1; workId <= last_worker; workId++)                   // for all workers, receive the results of counters
            {
                int words_counter, vowel_counter, consonant_counter;
                MPI_Recv(&words_counter, 1, MPI_INT, workId, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&vowel_counter, 1, MPI_INT, workId, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&consonant_counter, 1, MPI_INT, workId, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                
                updateFilesInfo(file_info , file_id, words_counter, vowel_counter, consonant_counter);  //update the values
            }
        }

        fclose(file);
    }
    moreWork = false;

    for (int workId = 1; workId < size; workId++){ // To finish workers
        MPI_Send(&moreWork, 1, MPI_C_BOOL, workId, 0, MPI_COMM_WORLD); // Send value to identify if will continue or not
    }
    printResults(file_info, nFiles);
}
