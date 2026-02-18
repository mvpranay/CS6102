#ifndef SHAKE256_H
#define SHAKE256_H

#include <stdint.h>
#include <stddef.h>

/**
 * SHAKE256 - Extendable-Output Function (XOF) based on Keccak
 * 
 * @param output: Buffer to store the output bytes
 * @param output_len: Number of bytes to generate
 * @param input: Input message
 * @param input_len: Length of input message in bytes
 */
void shake256(uint8_t *output, size_t output_len, 
              const uint8_t *input, size_t input_len);

#endif // SHAKE256_H
