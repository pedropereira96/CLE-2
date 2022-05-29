#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "shared_region.h"

void loadFilesInfo(int nFiles, char *filenames[], fileInfo *file)
{
    for (int i = 0; i < nFiles; i++)
    {
        file[i].fileId = i;
        file[i].name = filenames[i];
        file[i].vowel_counter = 0;
        file[i].consonant_counter = 0;
        file[i].word_counter = 0;
    }
}

void updateFilesInfo(fileInfo *file, int nfile, int word_counter, int vowel_counter, int consonant_counter)
{
    file[nfile].vowel_counter += vowel_counter;
    file[nfile].consonant_counter += consonant_counter;
    file[nfile].word_counter += word_counter;
}

void printResults(fileInfo *file_info, int nFiles)
{

    for (int file_id = 0; file_id < nFiles; file_id++)
    {

        printf("\n\nFile name: %s\n", file_info[file_id].name);
        printf("Total number of words = %d\n", file_info[file_id].word_counter);
        printf("N. of words beginning with a vowel = %d\n", file_info[file_id].vowel_counter);
        printf("N. of words ending with a consonant = %d", file_info[file_id].consonant_counter);
    }
}