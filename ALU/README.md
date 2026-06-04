# 🧮 4-Bit Integrated Circuit ALU

This directory contains the software simulation and hardware documentation for a **4-Bit Arithmetic Logic Unit (ALU)** designed from first principles for **CSE 210 (Computer Architecture Sessional)**, Section C2, Group 2.

The ALU accepts two 4-bit inputs ($A$ and $B$) and three selection/control lines ($cs_2, cs_1, cs_0$) to execute six different arithmetic and logical operations.

---

## 📋 Operation Specifications

The control inputs ($cs_2, cs_1, cs_0$) select the operation based on the following truth table:

| $cs_2$ | $cs_1$ | $cs_0$ | Operation | Description | Target Formula | Adder Input $X_i$ | Adder Input $Y_i$ | Carry-in ($C_{in}$) |
| :---: | :---: | :---: | :---: | :--- | :--- | :--- | :--- | :---: |
| `X` | `0` | `0` | **Sub** | Subtraction | $A - B$ | $A_i$ | $\bar{B_i}$ | `1` |
| `X` | `0` | `1` | **Transfer A** | Direct output of input $A$ | $A$ | $A_i$ | `0` | `0` |
| `0` | `1` | `0` | **Add with Carry** | Addition with carry-in set | $A + B + 1$ | $A_i$ | $B_i$ | `1` |
| `0` | `1` | `1` | **Increment A** | Increments input $A$ by 1 | $A + 1$ | $A_i$ | `0` | `1` |
| `1` | `1` | `0` | **AND** | Bitwise logical AND | $A \wedge B$ | $A_i \wedge B_i$ | `0` | `0` |
| `1` | `1` | `1` | **Decrement A** | Decrements input $A$ by 1 | $A - 1$ | $A_i$ | `1111` | `0` |

*Note: For the logical **AND** operation ($110$), the logical unit bypasses the arithmetic adder.*

---

## 🧠 Control Unit Equations (K-Maps)

To map the selection bits ($cs_2, cs_1, cs_0$) to the inputs of the 4-bit parallel adder ($X_i, Y_i$) and the carry-in ($C_{in}$), we solved K-maps resulting in the following equations:
* **$S_1$ Selection Bit** (for input $Y_i$ multiplexer):
  $$S_1 = cs_1' \cdot cs_0' + cs_2 \cdot cs_1 \cdot cs_0$$
* **$S_2$ Selection Bit** (for input $Y_i$ multiplexer):
  $$S_2 = cs_1' \cdot cs_0' + cs_2' \cdot cs_0'$$
* **$C_{in}$ (Carry Input to Adder)**:
  $$C_{in} = cs_1' \cdot cs_0' + cs_2' \cdot cs_1$$
* **$S_0$ Selection Bit** (toggles between arithmetic adder and logical unit):
  $$S_0 = cs_2 \cdot cs_1 \cdot cs_0'$$

---

## 🏳️ Status Flags Logic

The ALU implements four standard status flags that dynamically update depending on the operation output:

1. **Carry ($C$)**: Represents the carry-out ($C_{out}$) of the 4-bit binary adder.
   * *Rule*: Active only during arithmetic operations. Forcefully reset to `0` during the bitwise AND logic operation.
2. **Sign ($S$)**: Indicates whether the result is negative.
   * *Formula*: Matches the most significant bit (MSB) of the final output ($O_3$).
3. **Overflow ($V$)**: Triggered when the signed result exceeds the 4-bit range $[-8, 7]$.
   * *Formula*: $V = X_3 \oplus Y_3 \oplus O_3 \oplus C_{out}$ (or $C_3 \oplus C_{out}$, where $C_3$ is the carry into the MSB).
   * *Rule*: Reset to `0` during logical operations.
4. **Zero ($Z$)**: Triggered when the final output is zero ($0000$).
   * *Formula*: $Z = \overline{O_3 \vee O_2 \vee O_1 \vee O_0}$

### ⚠️ Special Flag Adjustments (Sessional Guidelines)
* **NOT Operation**: Carry ($C$) and Overflow ($V$) flags are treated as **Don't Cares**. If the result becomes `0000`, the Zero ($Z$) flag must not change to `0` from `1`. The Sign ($S$) flag must remain unchanged or reflect the MSB.
* **AND/OR/XOR Operations**: Carry ($C$) and Overflow ($V$) flags must be forcefully cleared to `0`. Sign ($S$) and Zero ($Z$) flags update normally based on output bits.

---

## 🔌 Hardware Implementation & IC Count

The circuit design was highly optimized to use a minimal number of standard TTL IC chips. The physical implementation requires a total of **11 ICs**:

| IC Model | Quantity | Type | Function in ALU |
| :---: | :---: | :---: | :--- |
| **7483** | 1 | Arithmetic | 4-bit parallel binary adder with fast carry |
| **74157** | 3 | Multiplexer | Quad 2-to-1 multiplexers for selection of inputs ($X_i, Y_i$) |
| **7432** | 2 | Logic Gate | Quad 2-input OR gates |
| **7486** | 1 | Logic Gate | Quad 2-input XOR gates |
| **7404** | 2 | Logic Gate | Hex Inverter (NOT gates) |
| **7408** | 2 | Logic Gate | Quad 2-input AND gates |
| **Total** | **11** | | |

### 🛠️ Breadboard Construction Equipment
* **Protoboards**: 3 standard breadboards bridged together.
* **Power**: 1 rechargeable battery (VCC stabilized to 5V rail).
* **wiring**: 22-gauge solid hookup wires (color-coded).
* **Inputs & Indicators**: 15 Toggle switches (inputs $A, B$, and controls) and 20 LEDs with 220-ohm current-limiting resistors.
* **Troubleshooting**: Always use resistors with LEDs to prevent voltage/power drops across the breadboard. Bridge power lines across boards using short jumper wire loops.

---

## 🔬 Software Simulation & Files

* **Logisim Version**: generic 2.7.1
* **[Design/Ckt/Logisim_Circuit.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/ALU/Design/Ckt/Logisim_Circuit.circ)**: The complete interactive 4-bit ALU simulation.
* **[Design/Ckt/7400-lib.circ](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/ALU/Design/Ckt/7400-lib.circ)**: Custom IC library mapping logic blocks to standard IC layouts.
* **[Design/Report.pdf](file:///home/sudip-kumar-saha/Desktop/CSE-210-Computer%20Architecture/ALU/Design/Report.pdf)**: Compiled report detailing the circuit derivations, K-maps, block diagrams, and hardware photos.

*Note: During testing, the `7483` adder from the default library had a faulty $C_{out}$ port. This was resolved in the simulation by linking a custom adder circuit block.*
