#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "multiplier.h"
#include "multiplier_optimized.h"
#include "cycle_counter.h"

// Semihosting support for printf in QEMU
extern void initialise_monitor_handles(void);

// Simple UART output for QEMU (alternative to semihosting)
#define UART0_BASE 0x4000C000
#define UART0_DR (*(volatile uint32_t *)(UART0_BASE + 0x000))
#define UART0_FR (*(volatile uint32_t *)(UART0_BASE + 0x018))

static unsigned char a_vals[ITERATIONS];
static unsigned char b_vals[ITERATIONS];

void uart_putc(char c)
{
    while (UART0_FR & (1 << 5))
        ; // Wait until TX FIFO not full
    UART0_DR = c;
}

void uart_puts(const char *s)
{
    while (*s)
    {
        if (*s == '\n')
            uart_putc('\r');
        uart_putc(*s++);
    }
}

void print_cycles(const char *operation, uint32_t cycles)
{
    char buf[150];
    float c = (float)cycles / 4000.0f;
    sprintf(buf, "%s: \n \tTotal cycles(4000 iterations) : %lu \n\tAverage : %.3f\n\n", operation, cycles, c);
    uart_puts(buf);
}

void benchmark_multiplier(void)
{

    uint32_t cycles;
    volatile unsigned char res;

    srand(42);
    for (int i = 0; i < ITERATIONS; i++)
    {
        a_vals[i] = rand() % 16;
        b_vals[i] = rand() % 16;
    }

    uart_puts("\n=== GF(16) Multiplier Benchmark ===\n");
    uart_puts("Platform: ARM Cortex-M4 (QEMU)\n\n");
    enable_cycle_counter();

    // --- Benchmark Reference (Student) ---
    reset_cycle_counter();
    for (int i = 0; i < ITERATIONS; i++)
    {
        res = mul_f(a_vals[i], b_vals[i]);
    }
    cycles = get_cycles();
    uart_puts("\n========== Results ==========\n");
    print_cycles("\nYour Execution (mul_f)", cycles);

    // --- Benchmark Reference (Optimized) ---
    reset_cycle_counter();
    for (int i = 0; i < ITERATIONS; i++)
    {
        res = mul_f_optimized(a_vals[i], b_vals[i]);
    }
    cycles = get_cycles();
    print_cycles("Benchmark (mul_f_optimized)", cycles);
    uart_puts("===========================\n");

    (void)res;

    uart_puts("\n=== Benchmark Complete ===\n");
}

int main(void)
{
    benchmark_multiplier();

    while (1)
        ; // Hang
}