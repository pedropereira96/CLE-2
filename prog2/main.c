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

    if (size <= 1)
    {
        if (rank == 0)
            printf("Wrong number of processes! It must be greater than 1.\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    if (rank == 0)
    {
        struct timespec start, finish;                                            /* time limits */
        clock_gettime (CLOCK_MONOTONIC_RAW, &start);                              /* begin of measurement */

        char *files[argc - 2];

        for (int i = 1; i < argc; i++)
            files[i - 1] = argv[i];


        dispatcher(files, argc - 1, size);


        printf("\n\n\nTerminated.\n");
        clock_gettime (CLOCK_MONOTONIC_RAW, &finish);                                /* end of measurement */

        printf ("\nElapsed tim = %.6f s\n",  (finish.tv_sec - start.tv_sec) / 1.0 + (finish.tv_nsec - start.tv_nsec) / 1000000000.0);

    }
    else
    {
        worker();
    }

    MPI_Finalize();
    return 0;
}

void worker()
{
    bool hasWork;
    // This workers will receive the values of dispatcher process, and then send to dispatcher the values processed
    while (1)
    {
        MPI_Recv(&hasWork, 1, MPI_C_BOOL, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (!hasWork)
        {
            break;
        }

        int matrix_order;
        double *cells;
        int cellsSize;
        MPI_Recv(&matrix_order, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        cells = malloc(sizeof(double) * matrix_order * matrix_order);
        cellsSize = matrix_order * matrix_order;

        MPI_Recv(cells, cellsSize, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        double result = computeDeterminat(matrix_order, cells);

        MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
}

void dispatcher(char *files[], int nFiles, int size)
{

    bool hasMore = true;

    int last_worker = 1;

    fileInfo file_info[nFiles];

    loadFilesInfo(nFiles, files, file_info);

    for (int file_id = 0; file_id < nFiles; file_id++) // Loop for files
    {
        int matrix_process = 0;

        FILE *file = fopen(files[file_id], "rb"); // Open file

        if (file == NULL) // Check file
        {
            perror("File could not be read");
            exit(1);
        }

        int number_of_matrices, matrix_order; // Initialization variables for info files

        if (fread(&number_of_matrices, sizeof(int), 1, file) != 1) // Read number of matrices
        {
            perror("Error reading number of matrices!");
            exit(1);
        }

        if (fread(&matrix_order, sizeof(int), 1, file) != 1) // Read matrices orders
        {
            perror("Error reading matrices order!");
            exit(1);
        }

        updateFilesInfo(file_info, file_id, number_of_matrices, matrix_order);
        int x = 1;

        double *cells;

        hasMore = true;

        while (hasMore)
        {

            last_worker = 1;

            for (int workId = 1; workId < size; workId++)
            {

                if (matrix_process >= number_of_matrices)
                {
                    hasMore = !hasMore;
                    break;
                }

                cells = malloc(sizeof(double) * matrix_order * matrix_order);

                if (!fread(cells, sizeof(double), matrix_order * matrix_order, file))
                {
                    perror("Error reading values from matrix!\n");
                    exit(2);
                }

                last_worker++;
                matrix_process++;
                MPI_Send(&hasMore, 1, MPI_C_BOOL, workId, 0, MPI_COMM_WORLD);                        // Send value to identify if will continue or not
                MPI_Send(&matrix_order, 1, MPI_INT, workId, 0, MPI_COMM_WORLD);                      // Send matrix order value
                MPI_Send(cells, matrix_order * matrix_order, MPI_DOUBLE, workId, 0, MPI_COMM_WORLD); // Send cells
                free(cells);
            }

            for (int workId = 1; workId < last_worker; workId++)
            {
                double determinant;
                MPI_Recv(&determinant, 1, MPI_DOUBLE, workId, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // printf("Matrix  %d = %.3e\n", x, determinant);
                saveDeterminant(file_info, file_id, x, determinant);
                x++;
            }
        }

        fclose(file);
    }

    for (int workId = 1; workId < size; workId++) // To finish workers
        MPI_Send(&hasMore, 1, MPI_C_BOOL, workId, 0, MPI_COMM_WORLD);

    printResults(file_info, nFiles);
}
