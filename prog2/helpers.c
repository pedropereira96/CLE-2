

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



double computeDeterminat(int order_matrix, double *cells){
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