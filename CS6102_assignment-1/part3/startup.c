#include <stdint.h>

/* Linker symbols */
extern uint32_t _estack;
extern uint32_t _sdata, _edata, _sidata;
extern uint32_t _sbss, _ebss;

/* Core handlers */
void Reset_Handler(void);
void Default_Handler(void);
void SysTick_Handler(void);

/* Weak aliases MUST come after Default_Handler declaration */
void NMI_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)     __attribute__((weak, alias("Default_Handler")));

extern int main(void);

/* Vector table */
__attribute__((section(".isr_vector")))
void (* const vectors[])(void) = {
    (void (*)(void))(&_estack),  // 0: Initial SP
    Reset_Handler,               // 1: Reset
    NMI_Handler,                 // 2
    HardFault_Handler,           // 3
    MemManage_Handler,           // 4
    BusFault_Handler,            // 5
    UsageFault_Handler,          // 6
    0, 0, 0, 0,                  // 7–10 Reserved
    SVC_Handler,                 // 11
    DebugMon_Handler,            // 12
    0,                           // 13 Reserved
    PendSV_Handler,              // 14
    SysTick_Handler              // 15
};

/* Reset handler */
void Reset_Handler(void) {
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;

    while (dst < &_edata) {
        *dst++ = *src++;
    }

    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    main();
    while (1);
}

/* Default handler */
void Default_Handler(void) {
    while (1);
}
