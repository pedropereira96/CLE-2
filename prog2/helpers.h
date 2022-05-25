

/**
 * @brief Swap columns x<->y of 'm', with 'size' order 
 * 
 * \param m 
 * \param x 
 * \param y 
 * \param size 
 */
void swap_cols(double *m, int *x, int *y, int size);


/**
 * @brief Gauss Elimination Formula  
 * 
 * \param kj 
 * \param ki 
 * \param ii 
 * \param ij 
 */
void formula(double *kj, double ki, double ii, double ij);


double computeDeterminat(int order_matrix, double *cells);