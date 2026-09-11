# 4-Bit Arithmetic Logic Unit Designs

A collection of 4-bit ALU implementations built entirely from discrete 74HC-series logic ICs. The project begins with a minimal ripple-carry architecture assembled on breadboard, then evolves into an expanded design featuring a 3-bit operation code and a dedicated PCB layout demonstrating how arithmetic and logic operations can be derived from fundamental gate-level building blocks.

# Hardware-Minimalist 4-bit ALU
This ALU represents the most fundamental version of the project: a design built entirely from discrete logic ICs, assembled by hand using breadboards, wiring, and basic digital components. The core idea is to show how a complete arithmetic and logic system can emerge from a minimal set of building blocks, following the principles of NAND universality and simple ripple-carry computation.

At its heart, this design implements a 4-bit ripple-carry adder, where each bit calculates its sum and pass its carry to the next stage. While not the fastest architecture, its transparency makes it highly educational: every step of the computation from XOR-based addition to AND-driven carry generation is physically observable in the circuit.

Beyond addition and subtraction, the ALU includes a NAND-based logic path to highlight how a single universal gate can serve as the foundation for more complex operations. The result is a compact but fully functional ALU that exemplifies hardware minimalism, discrete logic design, and hands-on digital engineering.

## Technical Specifications

### Core Features
- **Data Width**: 4-bit parallel processing
- **Operations**: 
  - `00` - Bitwise NAND (A NAND B)
  - `01` - Arithmetic addition (A + B)
  - `11` - Arithmetic subtraction (A - B) using two's complement
- **Control Input**: 2-bit operation code
- **Output Flags**: 
  - **Carry Out** - For arithmetic overflow detection
  - **Zero Flag** - Indicates when result equals 0000
- **Design Philosophy**: Hardware minimalism via NAND universality

## Integrated Circuits Used
| IC          | Function        | Quantity | Purpose                                        |
| ----------- | --------------- | -------- | ---------------------------------------------- |
| **74HC86**  | Quad XOR Gate   | 4        | Full adder implementation and two's complement |
| **74HC08**  | Quad AND Gate   | 2        | Carry generation for arithmetic operations     |
| **74HC00**  | Quad NAND Gate  | 1        | NAND operation and control logic               |
| **74HC157** | 2:1 Multiplexer | 1        | Operation selection and output routing         |
| **74HC4002**| Dual 4-Input NOR Gate | 1        | Zero detection             |

**Total Components:** 9 ICs implementing a complete 4-bit ALU

## Hardware Implementation

### Circuit Design
<img width="5983" height="2897" alt="Main" src="https://github.com/user-attachments/assets/386595fb-e17a-4fdd-bfb6-af8aefbc1650" />

## Operation Details

#### 1. **Addition (01)**
- Implements 4-bit ripple-carry adder
- Uses XOR gates for sum, AND gates for carry
- Direct A + B computation

#### 2. **Subtraction (11)**
- Utilizes two's complement method
- Inverts B using XOR gates (B ⊕ 1)
- Adds 1 via carry-in for proper complement
- Result: A + (~B + 1) = A - B

#### 3. **NAND Operation (00)**
- Direct NAND gate implementation
- Demonstrates NAND universality principle
- Basis for emulating all other logic operations

### Flag Generation

#### **Zero Flag**
- Implemented using **74HC4002 Zero Detection Unit**
- Output: HIGH when result = 0000

#### **Carry Flag**
- Generated from the 4th full adder's carry-out
- Indicates arithmetic overflow in addition/subtraction

## Truth Table Examples

| A (bin) | B (bin) | Op | Result | Carry | Zero |
|---------|---------|----|--------|-------|------|
| 0101    | 0011    | 01 (Add) | 1000 | 0 | 0 |
| 0111    | 0010    | 11 (Sub) | 0101 | 1 | 0 |
| 1111    | 1111    | 00 (NAND) | 0000 | X | 1 |
| 0000    | 0000    | 01 (Add) | 0000 | 0 | 1 |

## Educational Value

This project demonstrates fundamental computer architecture concepts:

1. **NAND Universality**: All digital logic can be constructed from NAND gates
2. **Two's Complement Arithmetic**: How computers perform subtraction
3. **Hardware/Software Trade-off**: Why some operations are emulated in software
4. **Flag-Based Computing**: Essential for conditional operations in CPUs
5. **Ripple-Carry Design**: Basic but functional adder architecture

## ⚠️ Known Limitations

1. **Propagation Delay**: Ripple-carry design limits maximum speed
2. **Limited Operations**: Only 3 hardware operations
3. **No Overflow Flag**: Signed overflow detection not implemented

# Hardware-Minimalist 4-bit ALU (Modified 3-bit OP Code)

This updated version of the 4-bit minimalistic ALU introduces expanded operation selection while maintaining a fully discrete IC implementation. By replacing the 74HC157 with two 74HC153 multiplexers, the ALU can now handle five distinct operations controlled by a 3-bit operation code.

### Core Features

* **Data Width**: 4-bit parallel processing
* **Operations**:

  * `000` - Arithmetic Addition (A + B)
  * `100` - Arithmetic Subtraction (A - B) using two's complement
  * `X01` - Bitwise NAND (A NAND B)
  * `X10` - Bitwise XOR (A XOR B)
  * `X11` - Bitwise NOR (A NOR B)
* **Control Input**: 3-bit operation code
* **Output Flags**:

  * **Equal Flag** - Indicates when A equals B
  * **Zero Flag** - Indicates when ALU result equals 0000

### Integrated Circuits Used

| IC          | Function                   | Quantity | Purpose                                        |
| ----------- | -------------------------- | -------- | ---------------------------------------------- |
| **74HC86**  | Quad XOR Gate              | 5        | Full adder, two's complement, XOR operation    |
| **74HC08**  | Quad AND Gate              | 2        | Carry generation for arithmetic operations     |
| **74HC00**  | Quad NAND Gate             | 1        | NAND operation                                 |
| **74HC153** | Dual 4-input Multiplexer   | 2        | Selection between arithmetic and logic outputs |
| **74HC02**  | Quad NOR Gate              | 1        | NOR operation                                  |
| **74HC4002**| Dual 4-Input NOR Gate      | 1        | Zero and Equality detection                    |


**Total Components**: 12 ICs, implementing a complete 4-bit ALU with expanded functionality

## Hardware Implementation

### Circuit Design
<img width="10310" height="4993" alt="Main_1" src="https://github.com/user-attachments/assets/c5a1905a-8094-4885-8976-9bc89adcd3a2" />

### PCB Design in Kicad
<img width="1904" height="1006" alt="image" src="https://github.com/user-attachments/assets/43411e74-117a-4391-be49-2186bcb70619" />

After validating the architecture in simulation, the design was 
migrated from breadboard to a custom PCB manufactured via JLCPCB. 
Each IC is paired with a dedicated 100nF ceramic bypass capacitor
placed directly at the power pins to suppress switching noise. 
A 100µF bulk electrolytic capacitor stabilizes the 5V supply rail.

### Physical Build
<img width="1280" height="591" alt="image" src="https://github.com/user-attachments/assets/7ba15a83-eba1-46c5-832e-f99a556a512f" />

## Verification & Test Bench

To validate correct operation across all input combinations, a dedicated test bench was developed using an ESP32 microcontroller. The [`firmware`](./src/tester/src) exhaustively tests all 256 input combinations (A × B, 0–15) for each of the 8 opcode states, and verifies both output flags across all cases totaling 2,048 flag assertions for the Zero flag alone.
All five operations passed with 100% accuracy across every input combination. Both the Equal flag and Zero flag returned correct results in every tested case. Results are reported over Serial with ANSI color coding, distinguishing passing opcodes in green from partial matches in yellow. A 20×4 LCD display connected via I2C provides a real-time progress bar during testing and a summary screen upon completion.

### Operation Details

#### 1. **Arithmetic Addition (000)**

* Implements 4-bit ripple-carry adder
* Uses XOR gates for sum, AND gates for carry
* Direct A + B computation

#### 2. **Arithmetic Subtraction (100)**

* Utilizes two's complement method
* Inverts B using XOR gates and adds 1 via carry-in
* Result: A + (~B + 1) = A - B

#### 3. **Bitwise NAND (X01)**

* Uses 74HC00 to implement NAND logic

#### 4. **Bitwise XOR (X10)**

* Uses additional 74HC86 to implement XOR logic

#### 5. **Bitwise NOR (X11)**

* Uses 74HC02 to implement NOR logic

### Flag Generation (Implemented using **74HC4002**)

* **Zero Flag**: HIGH when ALU output is 0000
* **Equal Flag**: HIGH when A is equal to B

### Truth Table Examples

| A (bin) | B (bin) | Op         | Result | Zero | Equal |
| ------- | ------- | ---------- | ------ | ---- | ----- |
| 0101    | 0011    | 000 (Add)  | 1000   | 0    | 0     |
| 0111    | 0010    | 100 (Sub)  | 0101   | 0    | 0     |
| 1111    | 1111    | X01 (NAND) | 0000   | 1    | 1     |
| 1010    | 0101    | X10 (XOR)  | 1111   | 0    | 0     |
| 1100    | 1010    | X11 (NOR)  | 0001   | 0    | 0     |


# FPGA Implementation: Tang Nano 9K
This module ports the 4-bit ALU logic from discrete integrated circuits to a programmable Gowin GW1NR-LV9 FPGA on the Sipeed Tang Nano 9K board. This transformation demonstrates a key modern digital design workflow: translating a proven hardware concept into a compact, single-chip solution using Hardware Description Language (HDL). The implementation faithfully replicates the behavior of the original breadboard circuit, providing a direct comparison between discrete and programmable logic.

## Physical Prototype & Interfacing
The FPGA interacts with the physical world through a simple, hands-on interface built on a breadboard. This setup mirrors the experience of the discrete version while showcasing the FPGA's role as a universal logic device.

![20251219_232145 1](https://github.com/user-attachments/assets/a3ac4201-116e-4470-bcaf-a03b19d723fd)

## Design Philosophy & Notes
- **Architectural Fidelity**: The HDL description was structured to mirror the original ripple-carry data path, providing a clear one-to-one conceptual mapping between the discrete gate-level design and its HDL counterpart.
- **Resource Efficiency**: The entire system utilizes a very small fraction of the FPGA's available logic cells, demonstrating how a functional digital system can be miniaturized onto modern programmable logic.
- **Practical Focus**: The primary goal was a correct and understandable translation to HDL rather than performance optimization, maintaining the project's core educational value.

## Comparative Summary: Discrete vs. FPGA
This implementation highlights the practical differences between two hardware paradigms.

| Aspect | Discrete IC Implementation | FPGA Implementation |
| :--- | :--- | :--- |
| **Component Count** | 9 to 12 ICs, extensive wiring. | **1** primary chip. |
| **Design Method** | Wiring logic gates on a breadboard. | Writing and synthesizing HDL code. |
| **Flexibility** | Fixed; changes require physical rewiring. | **Reconfigurable** via code upload. |
| **Debugging** | Direct probing of every net with a multimeter. | Indirect, relying on synthesis reports and output observation. |
| **Core Experience** | Understanding gate-level data flow and propagation delay. | Learning the FPGA toolchain and HDL design patterns. |

## License
Licensed under the **Solderpad Hardware License v2.1**.  
See the [LICENSE](LICENSE-HARDWARE) file for full terms.

## Contact
If you have any questions or suggestions, feel free to reach out:
- **GitHub:** [Neowizen](https://github.com/Yamil-Serrano)
