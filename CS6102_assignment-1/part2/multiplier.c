#include "multiplier.h"
#include "stdio.h"

/**
 * GF(16) multiplication
 * Mod Polynomial: x^4 + x + 1 (0x13)
 */
static char exp_table[15] = {1, 2, 4, 8, 3, 6, 12, 11, 5, 10, 7, 14, 15, 13, 9};
static char log_table[16] = {0, 0, 1, 4, 2, 8, 5, 10, 3, 14, 9, 7, 6, 13, 11, 12};

unsigned char mul_f(unsigned char a, unsigned char b)
{
    if (a == 0 | b == 0) {
        return 0;
    }
    char log_a = log_table[a];
    char log_b = log_table[b];
    char log_result = (log_a + log_b) % 15;
    return exp_table[log_result];
}