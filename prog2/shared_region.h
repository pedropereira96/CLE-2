#ifndef SHARED_REGION_H
#define SHARED_REGION_H

#define NO_MORE_DATA -1
#define CHUNK_SIZE 10


/**
 * \brief structure to matrix, save all cells of matrix, order, file identification and id on file
 * 
 */
typedef struct matrix_structure
{
  int id;                             //file identification
  int matrix_id;                      //matrix identification
  double *cells;                      //matrix cells
  int order_matrix;                   //matrix order
} matrix_structure;


/**
 * \brief structure to chunks, save CHUNK_SIZE matrices 
 * 
 */
typedef struct chunk_structure
{
  matrix_structure *matrices[CHUNK_SIZE];   //matrix info
  unsigned int next_enter;            //next index to insert new matrix
  unsigned int next_process;          //next index to calculate/process
  unsigned int counter;               //counter 
} chunk_structure;



void init_shared_region(chunk_structure *chunk);
void put_matrix(chunk_structure *chunk, matrix_structure *mat);
matrix_structure *get_matrix(chunk_structure *chunk);

#endif