#ifndef CYCLE_COUNTER_H
#define CYCLE_COUNTER_H

#include <stdint.h>

/* SysTick registers */
#define SYSTICK_CTRL (*(volatile uint32_t*)0xE000E010)
#define SYSTICK_LOAD (*(volatile uint32_t*)0xE000E014)
#define SYSTICK_VAL  (*(volatile uint32_t*)0xE000E018)

/* SysTick CTRL bits */
#define SYSTICK_ENABLE     (1 << 0)
#define SYSTICK_TICKINT    (1 << 1)
#define SYSTICK_CLKSOURCE  (1 << 2)

/* 24-bit SysTick */
#define SYSTICK_MAX 0x00FFFFFF

/* Overflow counter (incremented in ISR) */
static volatile uint32_t systick_overflows = 0;

/* -------------------------------------------------- */
/* Initialize SysTick as a free-running cycle counter */
/* -------------------------------------------------- */
static inline void cycle_counter_init(void) {
    SYSTICK_CTRL = 0;                  // Disable SysTick
    SYSTICK_LOAD = SYSTICK_MAX;         // Max reload
    SYSTICK_VAL  = 0;                   // Clear current value
    SYSTICK_CTRL = SYSTICK_ENABLE |
                   SYSTICK_TICKINT |
                   SYSTICK_CLKSOURCE;  // Enable + IRQ + CPU clock
}

/* -------------------------------------------------- */
/* Read 64-bit cycle count (QEMU-safe)                */
/* -------------------------------------------------- */
static inline uint64_t get_cycles(void) {
    uint32_t over1, over2, val;

    /* Ensure atomic read across wrap */
    do {
        over1 = systick_overflows;
        val   = SYSTICK_VAL;
        over2 = systick_overflows;
    } while (over1 != over2);

    /* SysTick counts DOWN */
    uint32_t ticks = (SYSTICK_MAX - val);

    return ((uint64_t)over1 * (SYSTICK_MAX + 1)) + ticks;
}

/* -------------------------------------------------- */
/* SysTick interrupt handler                          */
/* -------------------------------------------------- */
void SysTick_Handler(void) {
    systick_overflows++;
}




#endif /* CYCLE_COUNTER_H */

