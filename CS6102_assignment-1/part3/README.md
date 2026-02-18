# MAYO Key Expansion Benchmark

This repository contains a benchmarking setup for **part of the secret key expansion routine** used in the **MAYO post-quantum digital signature scheme**.  
The goal of this assignment is to study and benchmark **finite-field arithmetic**, not to implement the full cryptographic construction.

---

## Overview

The MAYO signature scheme defines a secret key expansion algorithm in **Algorithm 6** of the official specification.  
In this lab, you are provided with a **partial implementation** of this algorithm and are required to complete **only a well-defined arithmetic component** of it.

📄 **Specification reference:**  
https://csrc.nist.gov/csrc/media/Projects/pqc-dig-sig/documents/round-1/spec-files/mayo-spec-web.pdf

---

## Prerequisites

Before running the benchmark tests, install the following dependencies:

```bash
sudo apt update && sudo apt install gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi qemu-system-arm gdb-multiarch build-essential make
```

These tools are required to cross-compile the code for ARM targets and to execute the benchmarks using **QEMU**.

---

## Assignment Task

You will work with the following function defined in `mayo.c`:

```c
expandSK(...)
```

This function corresponds to the secret key expansion routine described in **Algorithm 6** of the MAYO specification.

---

## Scope of Implementation (Read Carefully)

⚠️ **You are NOT required to implement the complete `expandSK` function.**

For this assignment, you must:

- Implement **only the GF(2¹⁶) multiplication logic** used inside `expandSK`
- Use the **GF(2¹⁶) multipliers implemented in the previous part** of the assignment
- HINT: An optimized multiplier is implemented by the paper's authors. An unoptimized multiplier will not pass our (hidden) benchmark.
- The paper uses 64-bit values; however, since our evaluation is performed on QEMU, we adapt the implementation to use 16-bit values instead.
- Modify **only the code sections explicitly marked with `TODO` comments**
- Leave all other logic unchanged

The remaining parts of `expandSK` are intentionally provided or stubbed out so that the focus remains on **finite-field arithmetic and performance evaluation**.

Any modifications outside the marked TODO sections are **not required and discouraged**.

---

## Benchmarking Instructions

After completing the required TODO sections, you can benchmark the key expansion routine by running:

```bash
make run
```

This command will:

1. Build the project using the ARM cross-compiler  
2. Execute the binary under QEMU  
3. Report timing results for the key expansion function

---

## Submission Checklist

Before submitting your work, ensure that:

- ✅ Only `TODO`-marked sections were modified  
- ✅ GF(2¹⁶) multiplication is correctly implemented  
- ✅ The project builds without errors  
- ✅ `make run` executes successfully  
- PART 3 submission requires only the mayo.c file

---

## Notes

- This assignment is **not** about cryptographic security or correctness of the full MAYO scheme.
- The focus is on **implementation discipline**, **finite-field arithmetic**, and **benchmarking methodology**.
