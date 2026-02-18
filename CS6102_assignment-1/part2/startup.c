#include <stdint.h>

extern uint32_t _estack;
extern uint32_t _sdata, _edata, _sidata;
extern uint32_t _sbss, _ebss;

void Reset_Handler(void);

void Default_Handler(void);

extern int main(void);

__attribute__((section(".isr_vector"))) void (*const vectors[])(void) = {
    (void (*)(void))(&_estack), // Initial SP
    Reset_Handler,              // 0x04 Reset
    Default_Handler,            // 0x08 NMI
    Default_Handler,          // 0x0C HardFault
    Default_Handler,            // 0x10 MemManage
    Default_Handler,            // 0x14 BusFault
    Default_Handler,            // 0x18 UsageFault
    0,
    0,
    0,
    0,               // Reserved
    Default_Handler, // 0x2C SVC
    Default_Handler, // 0x30 DebugMon
    0,               // Reserved
    Default_Handler, // 0x38 PendSV
    Default_Handler, // 0x3C SysTick
    // Interrupt vectors continue...
};
void Reset_Handler(void)
{
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;

    // Copy data section
    while (dst < &_edata)
    {
        *dst++ = *src++;
    }

    // Zero BSS
    dst = &_sbss;
    while (dst < &_ebss)
    {
        *dst++ = 0;
    }

    main();
    while (1)
        ;
}

void Default_Handler(void)
{
    while (1)
        ;
}