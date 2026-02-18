#include "shake256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Keccak parameters
#define KECCAK_RATE 136      // 1088 bits / 8 = 136 bytes for SHAKE256
#define KECCAK_CAPACITY 512  // 512 bits
#define KECCAK_STATE_SIZE 200 // 1600 bits / 8 = 200 bytes

// Round constants for Keccak-f[1600]
static const uint64_t RC[24] = {
    0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808AULL,
    0x8000000080008000ULL, 0x000000000000808BULL, 0x0000000080000001ULL,
    0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008AULL,
    0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000AULL,
    0x000000008000808BULL, 0x800000000000008BULL, 0x8000000000008089ULL,
    0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
    0x000000000000800AULL, 0x800000008000000AULL, 0x8000000080008081ULL,
    0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
};

// Rotation offsets for Keccak
static const int ROTATION_OFFSETS[25] = {
    0,  1,  62, 28, 27,
    36, 44, 6,  55, 20,
    3,  10, 43, 25, 39,
    41, 45, 15, 21, 8,
    18, 2,  61, 56, 14
};

/**
 * Load 64-bit little-endian value from byte array
 */
static inline uint64_t load64_le(const uint8_t *x) {
    return ((uint64_t)x[0])       | ((uint64_t)x[1] << 8)  |
           ((uint64_t)x[2] << 16) | ((uint64_t)x[3] << 24) |
           ((uint64_t)x[4] << 32) | ((uint64_t)x[5] << 40) |
           ((uint64_t)x[6] << 48) | ((uint64_t)x[7] << 56);
}

/**
 * Store 64-bit value to byte array in little-endian
 */
static inline void store64_le(uint8_t *x, uint64_t u) {
    x[0] = (uint8_t)u;
    x[1] = (uint8_t)(u >> 8);
    x[2] = (uint8_t)(u >> 16);
    x[3] = (uint8_t)(u >> 24);
    x[4] = (uint8_t)(u >> 32);
    x[5] = (uint8_t)(u >> 40);
    x[6] = (uint8_t)(u >> 48);
    x[7] = (uint8_t)(u >> 56);
}

/**
 * Rotate left operation
 */
static inline uint64_t rotl64(uint64_t x, int n) {
    return (x << n) | (x >> (64 - n));
}

/**
 * Keccak-f[1600] permutation
 */
static void keccak_f1600(uint64_t state[25]) {
    uint64_t C[5], D[5], B[25];
    
    for (int round = 0; round < 24; round++) {
        // Theta step
        for (int i = 0; i < 5; i++) {
            C[i] = state[i] ^ state[i + 5] ^ state[i + 10] ^ 
                   state[i + 15] ^ state[i + 20];
        }
        
        for (int i = 0; i < 5; i++) {
            D[i] = C[(i + 4) % 5] ^ rotl64(C[(i + 1) % 5], 1);
        }
        
        for (int i = 0; i < 25; i++) {
            state[i] ^= D[i % 5];
        }
        
        // Rho and Pi steps
        for (int i = 0; i < 25; i++) {
            int pi_index = (i % 5) * 5 + ((i / 5 + 3 * (i % 5)) % 5);
            B[pi_index] = rotl64(state[i], ROTATION_OFFSETS[i]);
        }
        
        // Chi step
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                state[i + 5 * j] = B[i + 5 * j] ^ 
                    ((~B[(i + 1) % 5 + 5 * j]) & B[(i + 2) % 5 + 5 * j]);
            }
        }
        
        // Iota step
        state[0] ^= RC[round];
    }
}

/**
 * SHAKE256 implementation
 */
void shake256(uint8_t *output, size_t output_len,
              const uint8_t *input, size_t input_len) {
    uint64_t state[25] = {0};
    uint8_t state_bytes[KECCAK_STATE_SIZE];
    size_t absorbed = 0;
    
    // Absorbing phase
    while (absorbed < input_len) {
        size_t block_size = (input_len - absorbed < KECCAK_RATE) ? 
                           (input_len - absorbed) : KECCAK_RATE;
        
        for (size_t i = 0; i < block_size; i++) {
            state_bytes[i] = input[absorbed + i];
        }
        
        // XOR input block into state
        for (size_t i = 0; i < block_size / 8; i++) {
            state[i] ^= load64_le(state_bytes + i * 8);
        }
        
        // Handle remaining bytes
        if (block_size % 8 != 0) {
            uint64_t partial = 0;
            for (size_t i = 0; i < block_size % 8; i++) {
                partial |= ((uint64_t)state_bytes[(block_size / 8) * 8 + i]) << (i * 8);
            }
            state[block_size / 8] ^= partial;
        }
        
        absorbed += block_size;
        
        if (block_size == KECCAK_RATE) {
            keccak_f1600(state);
        }
    }
    
    // Padding: 0x1F for SHAKE256
    size_t last_byte_pos = input_len % KECCAK_RATE;
    uint64_t padding = 0x1F;
    state[last_byte_pos / 8] ^= (padding << ((last_byte_pos % 8) * 8));
    state[(KECCAK_RATE - 1) / 8] ^= (0x80ULL << (((KECCAK_RATE - 1) % 8) * 8));
    
    keccak_f1600(state);
    
    // Squeezing phase (NO memcpy, NO temp buffer)
    size_t squeezed = 0;
    while (squeezed < output_len) {
    	size_t block_size = (output_len - squeezed < KECCAK_RATE) ?
			    (output_len - squeezed) : KECCAK_RATE;

    	size_t out = 0;
    	for (size_t i = 0; i < 25 && out < block_size; i++) {
        	uint64_t lane = state[i];

        	for (int b = 0; b < 8 && out < block_size; b++) {
            		output[squeezed + out] = (uint8_t)(lane & 0xFF);
            		lane >>= 8;
            		out++;
        	}
    	}

    	squeezed += block_size;

    	if (squeezed < output_len) {
        	keccak_f1600(state);
    	}
    }
}
