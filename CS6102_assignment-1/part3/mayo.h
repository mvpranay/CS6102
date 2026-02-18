#ifndef MAYO_H
#define MAYO_H

#include <stdint.h>
#include <stddef.h>

// ============================================================================
// MAYO Parameters
// ============================================================================

#define N 66            
#define M 64              
#define O_PARAM 8       
#define K 9             
#define Q 65536         
#define V (N - O_PARAM) 

// Matrix dimensions
#define P1_ROWS V
#define P1_COLS V
#define P2_ROWS V
#define P2_COLS O_PARAM
#define O_ROWS V
#define O_COLS O_PARAM
#define L_ROWS V
#define L_COLS O_PARAM


uint16_t gf65536_mul(uint16_t a, uint16_t b);
void gf65536_mat_transpose(const uint16_t *src, uint16_t *dst, 
                           int rows, int cols);
void gf65536_mat_add(const uint16_t *A, const uint16_t *B, uint16_t *C,
                     int rows, int cols);
void gf65536_mat_mul(const uint16_t *A, const uint16_t *B, uint16_t *C,
                     int rows_A, int cols_A, int cols_B);
int compute_L(const uint16_t *P1, const uint16_t *P2, 
               const uint16_t *O, uint16_t *L);

// ============================================================================
// Utility Functions (Provided)
// ============================================================================

void print_matrix_gf65536(const char *name, const uint16_t *matrix, 
                          int rows, int cols);
void init_random_matrix_p(uint16_t *matrix, int rows, int cols, 
                       const uint8_t *seed, size_t seed_len);

void init_random_matrix_p(uint16_t *matrix, int rows, int cols, 
                       const uint8_t *seed, size_t seed_len);

void init_random_matrix_l(uint16_t *matrix, int rows, int cols);

#endif
