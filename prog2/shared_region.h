#ifndef SHARED_REGION_H
#define SHARED_REGION_H



typedef struct fileInfo
{
  int fileId;           
  char *name;              
  int number_of_matrices;                    
  int matrix_order;              
  double *determinants;                    
} fileInfo;

void loadFilesInfo(int nFiles, char *filenames[], fileInfo *file);
void updateFilesInfo(fileInfo *file, int nfile, int counts, int order);
void saveDeterminant( fileInfo *file,int fileId, int matrix_id, double determinat);
void printResults (fileInfo *file_info, int nFiles);

#endif