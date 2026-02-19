#include "multiplier.h"
#include "stdio.h"

/**
 * GF(16) multiplication
 * Mod Polynomial: x^4 + x + 1 (0x13)
 */
unsigned char mul_f(unsigned char a, unsigned char b)
{
    
    unsigned char r0_2 = ((a & 2) * b) ^ ((a & 1) * b) ^ ((a & 4) * b) ^ ((a & 8) * b);
    return (r0_2 ^ (r0_2 >> 4 & 0xf) ^ (r0_2 & 0xf0) >> 3) & 0xf;
}

// int main() {
//     for (int i = 0; i < 16; i++) {
//         for (int j = 0; j < 16; j++) {
//             // %x prints lowercase hex
//             printf("RESULT: %x %x = %x\n", i, j, mul_f(i, j));
//         }
//     }
//     return 0;
// }