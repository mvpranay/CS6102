# Part2 - GF(16) Multiplier on ARM Cortex-M4

### Overview

<br>The multiplication is performed over $GF(2^4)$ using the irreducible polynomial:$$P(x) = x^4 + x + 1$$
In this assignment, you will implement a Galois Field $GF(16)$ multiplication function and compare its performance against a reference optimized implementation.
You will be working in a bare-metal environment using the QEMU emulator to simulate an ARM Cortex-M4 (MPS2/LM3S6965) microcontroller

## File structure :

- `multiplier.c` : **[Your Task]** Implement your $GF(16)$ multiplication here.
- `multiplier_optimized.o` : ARM-32 elf binary file which runs the optimized version of the multiplier.
  <br>**⚠️ [NOTE]: PLEASE DON'T DELETE THE ABOVE FILE**
  <br> ( `multiplier_optimized.h` is provided for the function reference)

- `Makefile` : To compile and link necessary code<br>

## Getting Started:

- Make sure to install the following:

```bash
# Install the ARM Cross-Compiler
sudo apt update
sudo apt install gcc-arm-none-eabi binutils-arm-none-eabi

# Install QEMU for ARM
sudo apt install qemu-system-arm
```

- Implement your code in `multiplier.c`
- Compile and Run using

```bash
 make
 make run
```

## ⚠️ Important Note:

- DO NOT modify the QEMU-related files: `startup.c`, `syscalls.c`, `stm32.ld`, `cycle_counter.h`, `benchmark.c`
