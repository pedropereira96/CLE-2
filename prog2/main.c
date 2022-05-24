#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include <stdbool.h> 

void dispatcher(char *files[], int nFiles, int size);
float computeDeterminat(int order_matrix, double *cells);



int main(int argc, char *argv[]){
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("size = %d\n",size);
    printf("\t\t ATUAL RANK = %d\n",rank);
    if (rank == 0 )
    {
        //Will do dispacher

        //Dispatcher will read files and send chunks



        char *files[argc-2];
        
        for (int i = 1; i< argc; i++){
            files[i-1]=argv[i];
            printf("%d - %s\n",i,files[i-1]);
        }

        dispatcher(files, argc-1, size);

    }else{
        //Will do workers
        
        bool hasWork = true; 
        //This workers wir receive the values of dispatcher process, and then send to dispatcher the values processed
        while (1)
        {
            MPI_Recv(&hasWork, 1, MPI_C_BOOL, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (!hasWork){
                break;
            }


            int matrix_order;
            double *cells;
            int cellsSize;
            MPI_Recv (&matrix_order,1, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

            cells = malloc(sizeof(double) * matrix_order * matrix_order);

            MPI_Recv(cells, cellsSize, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            double result = computeDeterminat(matrix_order, cells);

            MPI_Send(&result, 1, MPI_DOUBLE,0,0,MPI_COMM_WORLD);
        }
        

    }

    printf("\n\nTERMINA\n\n\n\n");
    MPI_Finalize();
    return 0;
}



void dispatcher(char *files[], int nFiles, int size){

    printf("Entrou\n");
    bool hasMore = true;

    int last_worker = 1;

    for (int file_id = 0; file_id < nFiles; file_id++)              //Loop for files
    {
        printf("FILES %d\n", file_id);
        int matrix_process = 0;

        FILE *file = fopen(files[file_id], "rb");               //Open file
        

        if (file == NULL)                                           // Check file
        {
            perror("File could not be read");
            exit(1);
        }

        int number_of_matrices, matrix_order;                       //Initialization variables for info files

        if (fread(&number_of_matrices, sizeof(int), 1, file) != 1)  //Read number of matrices
        {
            perror("Error reading number of matrices!");
            exit(1);
        }

        if (fread(&matrix_order, sizeof(int), 1, file) != 1)        //Read matrices orders
        {
            perror("Error reading matrices order!");
            exit(1);
        }

        
        int x = 1;

        double *cells;

        printf("Number of matrices = %d\n", number_of_matrices);
        printf("Matrix order = %d\n\n\n", matrix_order);
         hasMore = true;

        while (hasMore){

             last_worker = 1;

            for (int workId = 1; workId < size; workId++)
            {
                if (matrix_process >= number_of_matrices){
                    hasMore = !hasMore;
                    break;
                }
                int cellsSize = sizeof(double) * matrix_order * matrix_order;

                //printf("\t\t\t\t\tcellsSize = %d\n", cellsSize);
                cells = malloc(cellsSize);
                if (!fread(cells, sizeof(double), matrix_order * matrix_order, file) )
                {
                    perror("Error reading values from matrix!\n");
                    exit(2);
                }

                //printf("Process Matrix = %d\n", matrix_process );

                last_worker++;
                matrix_process++;

                //Enviar
                //ultima matrix = numero do processo
                MPI_Send(&hasMore, 1, MPI_C_BOOL, workId, 0, MPI_COMM_WORLD);
                MPI_Send(&matrix_order, 1, MPI_INT, workId, 0, MPI_COMM_WORLD);
                MPI_Send(cells, (cellsSize/sizeof(double)), MPI_DOUBLE, workId, 0, MPI_COMM_WORLD);
                //printf("send all from dispatcher on worker:  %d\n   \n" , workId);
                free(cells);
            }

            for (int workId = 1; workId < last_worker; workId++)
            {
                double determinant = 0;
                MPI_Recv(&determinant, 1, MPI_DOUBLE, workId, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                printf("Matrix  %d = %.3e\n", x, determinant);
                x++;
            }


        }

        printf("\n\nFINISH FILE\n\n\n");

        fclose(file);
    }
    for (int workId = 1; workId < size; workId++)
            MPI_Send(&hasMore, 1, MPI_C_BOOL, workId, 0, MPI_COMM_WORLD);


 printf("\n\nFINISH DISPATCHER\n\n\n");

}




/**
 * @brief Swap columns x<->y of 'm', with 'size' order 
 * 
 * \param m 
 * \param x 
 * \param y 
 * \param size 
 */
void swap_cols(double *m, int *x, int *y, int size)
{
    //Swap both cols 
    // for all lines
    for (int i = 0; i < size; i++)
    {
        double aux = m[size * i + (*x)];                        // get first value
        m[size * i + (*x)] = m[size * i + (*y)];                // update next cell value with previous cell value
        m[size * i + (*y)] = aux;                               // update previous with next
    }
}

/**
 * @brief Gauss Elimination Formula  
 * 
 * \param kj 
 * \param ki 
 * \param ii 
 * \param ij 
 */
void formula(double *kj, double ki, double ii, double ij)
{
    // gauss elimination formula
    *kj -= ((ki / ii) * ij);
}



float computeDeterminat(int order_matrix, double *cells){
      //determinant result initialization
        double result = 1;                                              // don't start with zero because the multiplication will wrong


        for (int x = 0; x < order_matrix; x++)                  //loop for lines
        {
            int diagonal = order_matrix * x + x;                // get diagonal to x line
            if (cells[diagonal] == 0)                           //check if diagonal is zero
            {
                for (int y = x + 1; y < order_matrix; y++)      //this next loop, will swap cols until diagonal not be zero
                {
                    if (cells[diagonal] != 0)
                    {
                        swap_cols(cells, &x, &y, order_matrix);      //Swap col x<->y
                        break;
                    }
                }
            }

            for (int y = order_matrix - 1; y > x - 1; y--)      //Apply  gauss elimination
            {
                for (int k = x + 1; k < order_matrix; k++)
                {
                    //Apply formula gauss elimination
                    formula(&cells[order_matrix * k + y], cells[order_matrix * k + x], cells[order_matrix * x + x], cells[order_matrix * x + y]);
                }
            }

            /*if after exchanging col and applying formula, the diagonal value is still zero,
            it is because the matrix is ​​a singular matrix and the determinant is zero*/
            if (cells[order_matrix * x + x] == 0)
                return 0;

            // do multiplication diagonal to determine determinant
            result *= cells[order_matrix * x + x];
        }

        return result;
}