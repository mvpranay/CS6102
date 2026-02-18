#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "mayo.h"
#include "cycle_counter.h"

// Semihosting support for printf in QEMU
extern void initialise_monitor_handles(void);

// Simple UART output for QEMU (alternative to semihosting)
#define UART0_BASE 0x4000C000
#define UART0_DR   (*(volatile uint32_t*)(UART0_BASE + 0x000))
#define UART0_FR   (*(volatile uint32_t*)(UART0_BASE + 0x018))

void uart_putc(char c) {
    while (UART0_FR & (1 << 5)); // Wait until TX FIFO not full
    UART0_DR = c;
}

void uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s++);
    }
}   

void print_cycles(const char *label, uint32_t cycles) {
    uart_puts(label);
    uart_puts(": ");
    uart_put_u32(cycles);
    uart_puts(" cycles\n");
}

void *memset(void *s, int c, size_t n) {
    unsigned char *p = s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

void uart_put_u32(uint32_t x) {
    char buf[10];
    int i = 0;

    if (x == 0) {
        uart_putc('0');
        return;
    }

    while (x) {
        buf[i++] = '0' + (x % 10);
        x /= 10;
    }
    while (i--) uart_putc(buf[i]);
}


int benchmark_mayo(void) {
    // Allocate buffers for MAYO operations
    // Adjust sizes based on your MAYO parameter set
    int ret;
    uint32_t cycles;
    uint16_t P1[P1_ROWS * P1_COLS];
    uint16_t P2[P2_ROWS * P2_COLS];
    uint16_t O[O_ROWS * O_COLS];
    uint16_t L[L_ROWS * L_COLS];
    // Initialize matrices with random data
    init_random_matrix_p(P1, P1_ROWS, P1_COLS, (uint8_t *)"P1Seed", 6);
    init_random_matrix_p(P2, P2_ROWS, P2_COLS, (uint8_t *)"P2Seed", 6);
    init_random_matrix_o(O, O_ROWS, O_COLS, (uint8_t *)"OSeed", 5);
    init_random_matrix_l(L, L_ROWS, L_COLS);
    
    // Benchmark: Key Generation
    // uart_puts("Running key generation...\n");
    cycle_counter_init();
    
    
    ret = compute_L(P1,P2,O,L);
    cycles = get_cycles();
    
    if (ret == 0) {
        return cycles;
    } else {
        uart_puts("Key generation FAILED!\n");
        return -1;
    }
    

}

int main(void) {
    // Use semihosting if available, otherwise UART
    #ifdef USE_SEMIHOSTING
    initialise_monitor_handles();
    printf("Starting MAYO benchmark with semihosting...\n");
    #endif
    uart_puts("\n=== MAYO PQC Benchmark ===\n");
    uart_puts("Platform: ARM Cortex-M4 (QEMU)\n\n");

    int cycle_avg = 0;
    for(int i =0;i<100;i++) cycle_avg += benchmark_mayo();
    
    cycle_avg = cycle_avg/100;
    print_cycles("Key Generation", cycle_avg);
    uart_puts("\n=== Benchmark Complete ===\n");
    // Exit QEMU cleanly
    #ifdef USE_SEMIHOSTING
    return 0;
    #else
    while (1); // Hang
    #endif
}
