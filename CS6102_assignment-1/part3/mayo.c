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
    uint8_t *random_bytes[2 * num_elements];
    
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
    return 0;
}

// ============================================================================
// TODO : Matrix Operations
// ============================================================================

void gf65536_mat_transpose(const uint16_t *src, uint16_t *dst,
                           int rows, int cols) {
    // TODO STUDENTS: Implement matrix transpose
}

void gf65536_mat_add(const uint16_t *A, const uint16_t *B, uint16_t *C,
                     int rows, int cols) {
    // TODO STUDENTS: Implement matrix addition over GF(2^16)
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
    return 0;
}
