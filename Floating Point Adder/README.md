# 🔢 32-Bit Floating Point Adder (FPA)

This directory contains a complete Logisim sessional simulation for a **32-Bit Floating Point Adder** designed by Section C2, Group 7 to execute addition and subtraction of floating-point numbers.

---

## 📐 Custom Floating-Point Format

The custom 32-bit floating-point layout splits the word as follows:

| Bit Range | Field | Size | Description |
| :---: | :---: | :---: | :--- |
| **31** | **Sign ($S$)** | 1 bit | `0` for positive numbers, `1` for negative numbers. |
| **30 – 22** | **Exponent ($E$)** | 9 bits | Biased exponent. Bias is $2^{9-1} - 1 = 255$. |
| **21 – 0** | **Fraction / Mantissa ($F$)** | 22 bits | Fractional part of the significand (excluding implicit leading bit). |

### 🔢 IEEE 754 Encoding & Special Values

Special numbers are encoded depending on the exponent field ($E$) and the fraction ($F$):

* **Zero ($0.0$)**: Exponent $E = 0$, Fraction $F = 0$.
* **Denormalized Numbers**: Exponent $E = 0$, Fraction $F \neq 0$. Represents gradual underflow:
  $$\text{Value} = (-1)^S \times (0 + F) \times 2^{-254}$$
* **Normalized Numbers**: Exponent $1 \le E \le 510$, Fraction $F$ is arbitrary:
  $$\text{Value} = (-1)^S \times (1.F) \times 2^{E - 255}$$
* **Infinity ($\pm\infty$)**: Exponent $E = 511$, Fraction $F = 0$.
* **NaN (Not a Number)**: Exponent $E = 511$, Fraction $F \neq 0$.

---

## 🔄 Addition & Subtraction Algorithm Flow

1. **Input Processing**: Extract sign, exponent, and fraction bits from inputs $A$ and $B$. Append the hidden bit (`1` for normalized, `0` for denormalized) to create 32-bit significands.
2. **Exponent Comparison**: Calculate the difference between exponents $|E_A - E_B|$.
3. **Radix Point Alignment**: Right-shift the significand of the operand with the smaller exponent by the calculated difference.
4. **Significand Arithmetic**: Add or subtract the aligned significands based on the signs.
5. **Initial Normalization**: Locate the first set bit of the result. Left-shift (or right-shift in case of addition carry) and update the exponent accordingly.
6. **Rounding**: Implement **Round-to-Even** on the 22-bit fraction using three status bits:
   * **Guard ($G$)**: The 23rd bit of the intermediate significand.
   * **Round ($R$)**: The 24th bit of the intermediate significand.
   * **Sticky ($S$)**: The logical OR of all bits from the 25th bit onwards.
   * **Round Up Flag Formula** (derived via K-Map):
     $$\text{flag} = G(M + R + S)$$
     *where $M$ is the 22nd bit (the LSB of the fraction).*
7. **Post-Rounding Normalization**: If rounding results in a carry-out, shift the significand right by 1 and increment the exponent.
8. **Exception Check**: Set status flags for **Overflow, Underflow, NaN, Infinity, and Denormalized**.
9. **Output Formatting**: Combine the sign bit, the 9-bit exponent, and the 22-bit fraction into the final 32-bit word.

---

## 📦 Logisim Modules breakdown

The project is modularized into these individual circuit libraries:

* **[FPA.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/Floating%20Point%20Adder/FPA.circ)**: Top-level Floating Point Adder integrating all libraries.
* **[IPH.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/Floating%20Point%20Adder/IPH.circ)**: Input Handler. Contains the input splitter, exponent differentiator, and sign selector.
* **[ALU.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/Floating%20Point%20Adder/ALU.circ)**: 32-bit ALU to perform significand addition/subtraction.
* **[Significand Adder.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/Floating%20Point%20Adder/Significand%20Adder.circ)**: Manages sign logic and feeds operands to the ALU.
* **[Normalizer.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/Floating%20Point%20Adder/Normalizer.circ)**: First-level normalization unit.
* **[Rounder.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/Floating%20Point%20Adder/Rounder.circ)**: Implementation of the round-to-even K-map logic.
* **[RoundedNormalizer.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/Floating%20Point%20Adder/RoundedNormalizer.circ)**: Normalizes values if rounding causes an overflow.
* **[Checker.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/Floating%20Point%20Adder/Checker.circ)**: Contains helper blocks checking for zero or all ones.
* **[Flag.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/Floating%20Point%20Adder/Flag.circ)**: Flag checker that triggers signals for overflow, underflow, NaN, and denormalized outputs.
* **[leftShifterLib.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/Floating%20Point%20Adder/leftShifterLib.circ)** & **[rightShifterLib.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/Floating%20Point%20Adder/rightShifterLib.circ)**: Multiplexer-based barrel shifters supporting 1, 2, 4, 8, 16, or arbitrary (0-31) bit shifting.
* **[Output.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/Floating%20Point%20Adder/Output.circ)** & **[Output_p.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/Floating%20Point%20Adder/Output_p.circ)**: Formats the final 32-bit floating point structure.

---

## 🐍 Python Verification Tester (`tester/`)

To verify the hardware logic, a Python test bench matches Logisim output vectors:
* **[tester/adder.py](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/Floating%20Point%20Adder/tester/adder.py)**: Simulates the custom FPA algorithm in software.
* **[tester/gen.py](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/Floating%20Point%20Adder/tester/gen.py)**: Run scripts. Test files `a.txt`, `b.txt`, and `c.txt` contain input vectors which can be directly loaded into Logisim memory tables for verification.
* **To run**:
  ```bash
  cd "tester"
  python3 gen.py
  ```

---

## 📄 Project Documentation & LaTeX Report (`report/`)

* **[report/main.pdf](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/Floating%20Point%20Adder/report/main.pdf)**: Compiled sessional assignment report.
* **[report/main.tex](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/Floating%20Point%20Adder/report/main.tex)**: LaTeX document source.
* **Compile instructions**:
  ```bash
  cd "report"
  latexmk -pdf -synctex=1 main.tex
  ```
