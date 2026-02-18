#ifndef CYCLE_COUNTER_H
#define CYCLE_COUNTER_H

#include <stdint.h>

// Use SysTick as a cycle approximation for QEMU
#define SYSTICK_CTRL    (*(volatile uint32_t*)0xE000E010)
#define SYSTICK_LOAD    (*(volatile uint32_t*)0xE000E014)
#define SYSTICK_VAL     (*(volatile uint32_t*)0xE000E018)

// Assume 16MHz clock (QEMU default for lm3s6965evb)
#define CPU_FREQ_HZ 16000000

static volatile uint32_t systick_overflows = 0;
static volatile uint32_t last_systick_val = 0;

void SysTick_Handler(void) {
    systick_overflows++;
}

static inline void enable_cycle_counter(void) {
    // Configure SysTick for maximum reload value
    SYSTICK_LOAD = 0x00FFFFFF; // 24-bit max
    SYSTICK_VAL = 0;
    SYSTICK_CTRL = 0x07; // Enable, interrupt, use CPU clock
    systick_overflows = 0;
}

static inline void reset_cycle_counter(void) {
    systick_overflows = 0;
    SYSTICK_VAL = 0;
    last_systick_val = SYSTICK_LOAD;
}

static inline uint32_t get_cycles(void) {
    uint32_t current_val = SYSTICK_VAL;
    uint32_t elapsed = last_systick_val - current_val;
    
    if (current_val > last_systick_val) {
        // Wrapped around
        elapsed = last_systick_val + (SYSTICK_LOAD - current_val);
    }
    
    uint32_t total = (systick_overflows * SYSTICK_LOAD) + elapsed;
    last_systick_val = current_val;
    
    return total;
}

#endif // CYCLE_COUNTER_H