#ifndef SHARED_REGION_H
#define SHARED_REGION_H

#define CHUNK_SIZE 52

typedef struct fileInfo
{
  int fileId;           
  char *name;              
  int vowel_counter;                    
  int consonant_counter;              
  int word_counter;                    
} fileInfo;

void loadFilesInfo(int nFiles, char *filenames[], fileInfo *file);
void updateFilesInfo(fileInfo *file, int nfile, int word_counter, int vowel_counter, int consonant_counter);
void printResults (fileInfo *file_info, int nFiles);

#endif