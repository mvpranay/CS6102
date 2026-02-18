# Part1 - GF(16) Multiplier implementation in Verilog

## Overview
The multiplication is performed over 
GF(2<sup>4</sup>) using the irreducible polynomial:$$P(x) = x^4 + x + 1$$

In this assignment, you will implement a Galois Field  GF(2<sup>4</sup>) multiplier in Verilog, using **synthesizable combinational logic** only and test its correctness using the testbench provided in ```testbench.sv```. 


## File Structure

- ```mulitiplier.v``` :  **[Your Task]** Implement your 
GF(2<sup>4</sup>) multiplier here.

**DO NOT CHANGE** the name of the multplier module and the ports.


## Build Instructions

```
    iverilog multiplier.v -o [object file name]
    vvp [object file name]
```

## Validation of the multiplier

use the values provided in `reference.txt` to evaluate the correctness of your implementation