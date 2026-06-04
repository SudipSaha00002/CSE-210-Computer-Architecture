# ⚡ 8-Bit Custom MIPS CPU

This directory contains the design files, control logic, and pipeline units for a custom **8-Bit MIPS-like Processor** with a **20-bit Instruction Word** implemented in both single-cycle and pipelined architectures using Logisim.

---

## 📐 Instruction Set Architecture (ISA) & Opcode Mapping

The processor uses a 20-bit instruction format. The most significant 4 bits ($19\text{--}16$) define the **Opcode**:

| Opcode (Hex) | Mnemonic | Type | Syntax | Description | Control Bit Sequence |
| :---: | :--- | :---: | :--- | :--- | :--- |
| `0` | **lw** | I | `lw $rt, offset($rs)` | Load Word from memory | `0110100100` |
| `1` | **sw** | I | `sw $rt, offset($rs)` | Store Word to memory | `0001000100` |
| `2` | **subi** | I | `subi $rt, $rs, imm` | Subtract Immediate | `0100000100` |
| `3` | **ori** | I | `ori $rt, $rs, imm` | Bitwise OR Immediate | `0100000100` |
| `4` | **add** | R | `add $rd, $rs, $rt` | Addition | `0100001000` |
| `5` | **nor** | R | `nor $rd, $rs, $rt` | Bitwise NOR | `0100001000` |
| `6` | **andi** | I | `andi $rt, $rs, imm` | Bitwise AND Immediate | `0100000100` |
| `7` | **srl** | Shift | `srl $rd, $rt, shamt` | Shift Right Logical | `0100001001` |
| `8` | **sll** | Shift | `sll $rd, $rt, shamt` | Shift Left Logical | `0100001001` |
| `9` | **sub** | R | `sub $rd, $rs, $rt` | Subtraction | `0100001000` |
| `a` | **and** | R | `and $rd, $rs, $rt` | Bitwise AND | `0100001000` |
| `b` | **addi** | I | `addi $rt, $rs, imm` | Add Immediate | `0100000100` |
| `c` | **or** | R | `or $rd, $rs, $rt` | Bitwise OR | `0100001000` |
| `d` | **beq** | Branch| `beq $rs, $rt, label` | Branch if Equal | `0000010010` |
| `e` | **bneq**| Branch| `bneq $rs, $rt, label`| Branch if Not Equal | `0000010000` |
| `f` | **j** | Jump | `j label` | Unconditional Jump | `1000000000` |

---

## 🗄️ Register File Layout

The processor has **8 registers** ($zero$ and $t_0\text{--}t_4$ are general-purpose, $sp$ is the stack pointer, and $ra$ is the return address register). They are mapped using 4 bits in machine code (registers $0\text{--}7$):

| Register Name | Code (Hex) | Binary | Description / Usage |
| :---: | :---: | :---: | :--- |
| **$zero** | `0` | `0000` | Hardwired Constant Zero |
| **$t0** | `1` | `0001` | Temporary Register 0 |
| **$t1** | `2` | `0010` | Temporary Register 1 |
| **$t2** | `3` | `0011` | Temporary Register 2 |
| **$t3** | `4` | `0100` | Temporary Register 3 |
| **$t4** | `5` | `0101` | Temporary Register 4 |
| **$sp** | `6` | `0110` | Stack Pointer |
| **$ra** | `7` | `0111` | Return Address (for subroutine jumps) |

---

## 📌 Instruction Formats (20-Bit Words)

Instructions are represented as 5-character hex strings in Logisim raw memory files:

1. **R-Type** (e.g., `add $rd, $rs, $rt`):
   $$\text{Instruction} = [\text{Opcode: } 4\text{b}] \mid [\text{rs: } 4\text{b}] \mid [\text{rt: } 4\text{b}] \mid [\text{rd: } 4\text{b}] \mid [0000\text{ funct: } 4\text{b}]$$
2. **I-Type** (e.g., `addi $rt, $rs, imm`):
   $$\text{Instruction} = [\text{Opcode: } 4\text{b}] \mid [\text{rs: } 4\text{b}] \mid [\text{rt: } 4\text{b}] \mid [\text{Immediate Value: } 8\text{b}]$$
3. **Shift Type** (e.g., `sll $rd, $rt, shamt`):
   $$\text{Instruction} = [\text{Opcode: } 4\text{b}] \mid [\text{rt: } 4\text{b}] \mid [0000\text{: } 4\text{b}] \mid [\text{rd: } 4\text{b}] \mid [\text{Shift Amount: } 4\text{b}]$$
4. **Jump Type** (e.g., `j label`):
   $$\text{Instruction} = [\text{Opcode: } 4\text{b}] \mid [\text{Target Address: } 8\text{b}] \mid [00000000\text{: } 8\text{b}]$$

---

## ⚡ Processor Datapaths & Control

The repository contains two CPU implementations in Logisim:

### 1. Single-Cycle Datapath
* **Files**: [MIPS_without_pipeline.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/MIPS/MIPS_without_pipeline.circ)
* Executes each instruction in a single clock cycle. Best for verification of basic ALU operations and control signals.

### 2. Pipelined Datapath
* **Files**: [MIPS_with_pipeline.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/MIPS/MIPS_with_pipeline.circ)
* Features 4 stages separated by registers ([Pipe_reg.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/MIPS/Pipe_reg.circ)): **Instruction Fetch (IF)**, **Instruction Decode/Register File (ID)**, **Execute (EX)**, and **Memory Access/Write Back (MEM/WB)**.
* **RAW Data Hazard Forwarding Unit** ([ex_forwarding_unit.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/MIPS/ex_forwarding_unit.circ), [RAAAW_forwarding.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/MIPS/RAAAW_forwarding.circ)): Dynamically forwards ALU values to bypass memory write delays.
* **Stall Control Unit** ([Load hazard detect.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/MIPS/Load%20hazard%20detect.circ), [Branch_Detection.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/MIPS/Branch_Detection.circ)): Detects load-use dependencies and injects pipeline bubbles (stalls) by pausing the PC and flushing registers.

---

## 🛠️ Assembler Tools (`test/`)

To compile assembly instructions into Logisim raw hex compatibility format:

### Python Assembler
* **File**: [test/assembler.py](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/MIPS/test/assembler.py)
* **Usage**:
  ```bash
  cd test
  python3 assembler.py <input_code.asm> <machine_code.txt>
  ```

### C++ Assembler
* **File**: [test/assembler.cpp](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/MIPS/test/assembler.cpp)
* **Usage**:
  ```bash
  cd test
  g++ assembler.cpp -o assembler
  ./assembler
  ```
  *(Reads from `Input.txt` and outputs `Output.txt` in `v2.0 raw` format).*

### Test Assembly File
* **File**: [test/testCase#.asm](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/MIPS/test/testCase%23.asm)
* Demonstrates loops, stack pushes/pops using `$sp`, logical shifting (`srl`/`sll`), and comparison branching (`beq`/`bneq`) to calculate Fibonacci numbers.
