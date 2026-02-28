#include "mayo.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern void shake256(uint8_t *output, size_t output_len, 
                      const uint8_t *input, size_t input_len);

// Initialize a matrix with random GF(2^16) elements using SHAKE256
void init_random_matrix_p(uint16_t *matrix, int rows, int cols, 
                        const uint8_t *seed, size_t seed_len) {
    size_t num_elements = rows * cols;
    size_t num_bytes = num_elements * 2; 
    uint8_t random_bytes[2 * num_elements]; 
    
    shake256(random_bytes, num_bytes, seed, seed_len);
    
    for (size_t i = 0; i < num_elements; i++) {
        matrix[i] = (uint16_t)random_bytes[2*i] | 
                   ((uint16_t)random_bytes[2*i + 1] << 8);
    }
}

void init_random_matrix_o(uint16_t *matrix, int rows, int cols, 
                        const uint8_t *seed, size_t seed_len) {
    size_t num_elements = rows * cols;
    size_t num_bytes = num_elements * 2; 
    uint8_t random_bytes[2 * num_elements];
    
    shake256(random_bytes, num_bytes, seed, seed_len);
    
    for (size_t i = 0; i < num_elements; i++) {
        matrix[i] = (uint16_t)random_bytes[2*i] | 
                   ((uint16_t)random_bytes[2*i + 1] << 8);
        matrix[i] %= 16;
    }
}

void init_random_matrix_l(uint16_t *matrix, int rows, int cols) {
    size_t num_elements = rows * cols;
    
    for (size_t i = 0; i < num_elements; i++) {

        matrix[i] = 0;
    }

}


// ============================================================================
// TODO : GF(2^16) Multiplication
// ============================================================================

uint16_t gf65536_mul(uint16_t a, uint16_t b) {
    // TODO STUDENTS: Implement GF(2^16) multiplication
    uint16_t X1 = (a & 0xFF00) >> 8;
    uint16_t X2 = (a & 0x00FF);
    
    uint16_t spaced_X1 = (X1 & 0x000F) | ((X1 & 0x00F0) << 4);
    uint16_t spaced_X2 = (X2 & 0x000F) | ((X2 & 0x00F0) << 4);

    uint16_t prod1 = (-(b & 1) & spaced_X1) 
                  ^ ((b & 2) * spaced_X1) 
                  ^ ((b & 4) * spaced_X1) 
                  ^ ((b & 8) * spaced_X1);

    uint16_t prod2 = (-(b & 1) & spaced_X2) 
                  ^ ((b & 2) * spaced_X2) 
                  ^ ((b & 4) * spaced_X2) 
                  ^ ((b & 8) * spaced_X2);

    uint16_t reduced1 = prod1 
                     ^ ((prod1 >> 4) & 0x0F0F) 
                     ^ ((prod1 & 0xF0F0) >> 3);
    
    uint16_t reduced2 = prod2
                     ^ ((prod2 >> 4) & 0x0F0F) 
                     ^ ((prod2 & 0xF0F0) >> 3);

    return (((reduced2 & 0x000F) | ((reduced2 >> 4) & 0x00F0)) << 8) | (reduced2 & 0x000F) | ((reduced2 >> 4) & 0x00F0);

    return 0;
}

// uint16_t mul_f(uint16_t a, uint16_t b)
// {
//     uint16_t r0_2 = ((a & 2) * b) ^ ((a & 1) * b) ^ ((a & 4) * b) ^ ((a & 8) * b);
//     return (r0_2 ^ (r0_2 >> 4) ^ (r0_2 & 0xf0) >> 3) & 0xf;
// }

// uint16_t gf65536_mul(uint16_t a, uint16_t b) {
//     // TODO STUDENTS: Implement GF(2^16) multiplication
//     return mul_f(a & 0xF, b) ^ (mul_f((a & 0xF0) >> 4, b) << 4) ^ (mul_f((a & 0xF00) >> 8, b) << 8) ^ (mul_f((a & 0xF000) >> 12, b) << 12);
// }

// ============================================================================
// TODO : Matrix Operations
// ============================================================================

// mat[r,c] of dim (rows, cols) => mat[r * cols + c]

void gf65536_mat_transpose(const uint16_t *src, uint16_t *dst,
                           int rows, int cols) {
    // TODO STUDENTS: Implement matrix transpose
    for (int row = 0; row < rows; row++){
        for (int col = 0; col < cols; col++){
            dst[col * rows + row] = src[row * cols +  col];
        }
    }
}

void gf65536_mat_add(const uint16_t *A, const uint16_t *B, uint16_t *C,
                     int rows, int cols) {
    // TODO STUDENTS: Implement matrix addition over GF(2^16)
    for (int r = 0; r < rows; r++){
        for (int c = 0; c < cols; c++){
            C[r * cols + c] = A[r * cols + c] ^ B[r * cols + c];
        }
    }
}

/**
 * Matrix Multiplication
 * 
 * PERFORMANCE CRITICAL !!!
 * This is the bottleneck of the computation.
 * Make sure to optimize this function.
 */

void gf65536_mat_mul(const uint16_t *A, const uint16_t *B, uint16_t *C,
                     int rows_A, int cols_A, int cols_B) {
    // TODO STUDENTS: Implement matrix multiplication over GF(2^16)
    memset(C, 0, rows_A * cols_B * sizeof(uint16_t));
    for (int r = 0; r < rows_A; r++){
        for (int c = 0; c < cols_B; c++){
            for (int i = 0; i < cols_A; i++){
                C[r * cols_B + c] ^= gf65536_mul(A[r * cols_A + i], B[i * cols_B + c]);
            }
        }
    }
}

// ============================================================================
// TODO: Compute L
// ============================================================================

/**
* Refer to parameters in mayo.h (these will be varied later so don't hard code the dimensions) 
* Matrix dimensions:
 * - P1: V × V (58 × 58) - upper triangular
 * - P2: V × O_PARAM (58 × 8)
 * - O:  V × O_PARAM (58 × 8)
 * - L:  V × O_PARAM (58 × 8)
 */
int compute_L(const uint16_t *P1, const uint16_t *P2,
               const uint16_t *O, uint16_t *L) {
    // TODO: Implement the L computation
    uint16_t temp[P1_ROWS * P1_COLS];
    gf65536_mat_transpose(P1, temp, P1_ROWS, P1_COLS);
    gf65536_mat_add(P1, temp, L, P1_ROWS, P1_COLS);
    gf65536_mat_mul(L, O, temp, P1_ROWS, P1_COLS, O_COLS);
    gf65536_mat_add(temp, P2, L, P2_ROWS, P2_COLS);
    return 0;
}
