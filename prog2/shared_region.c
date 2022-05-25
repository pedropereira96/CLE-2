#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "shared_region.h"

void loadFilesInfo(int nFiles, char *filenames[], fileInfo *file) 
{
    for (int i=0; i<nFiles; i++) 
    {     
        file[i].fileId = i;
        file[i].name = filenames[i];
    }
}



void updateFilesInfo(fileInfo *file, int nfile, int counts, int order) 
{
  
    file[nfile].number_of_matrices = counts;
    file[nfile].matrix_order = order;    
    file[nfile].determinants = malloc(sizeof(double) * counts);
    
}


void saveDeterminant( fileInfo *file,int fileId, int matrix_id, double determinat) 
{
    file[fileId].determinants[matrix_id] = determinat;           
}


void printResults (fileInfo *file_info, int nFiles){

for (int file_id = 0; file_id < nFiles; file_id++){
        printf("\n\n\n\nNumber of matrices to be read = %d\n",file_info[file_id].number_of_matrices);
        printf("Matrices order = %d\n\n", file_info[file_id].matrix_order);

        for (int i = 0; i < file_info[file_id].number_of_matrices; i++)
        {
            printf("Processing matrix %d\n",i+1);
            printf("The determinant is %.3e\n",file_info[file_id].determinants[i+1]);
        }
        
    }  
}