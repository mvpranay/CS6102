#include "multiplier.h"
#include "stdio.h"

/**
 * GF(16) multiplication
 * Mod Polynomial: x^4 + x + 1 (0x13)
 */
unsigned char mul_f(unsigned char a, unsigned char b)
{
    // Write your code here
    unsigned char p = 0;
    
    // Unrolled loop for 4 bits (No branches)
    // Bit 0
    p = (b & 1) ? a : 0;
    a = (a << 1) ^ ((a & 0x8) ? 0x13 : 0);
    
    // Bit 1
    p ^= (b & 2) ? a : 0;
    a = (a << 1) ^ ((a & 0x8) ? 0x13 : 0);
    
    // Bit 2
    p ^= (b & 4) ? a : 0;
    a = (a << 1) ^ ((a & 0x8) ? 0x13 : 0);
    
    // Bit 3
    p ^= (b & 8) ? a : 0;
    return p & 0xF;
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