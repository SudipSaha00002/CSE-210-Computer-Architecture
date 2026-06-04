"""
program to generate control bits and ALU opcode from mips opcode.

lw   : 0
sw   : 1
subi : 2
ori  : 3
add  : 4
nor  : 5
andi : 6
srl  : 7
sll  : 8
sub  : 9
and  : a
addi : b
or   : c
beq  : d
bneq : e
j    : f

0 - j (1 for yes, 0 for no)
1 - Reg_Write (1 for yes, 0 for no)
2 - Mem_Read (1 for yes, 0 for no)
3 - Mem_Write (1 for yes, 0 for no)
4 - Mem_To_Reg (0 for ALU result, 1 for Memory data)
5 - Branching_Op (1 for yes, 0 for no)
6 - Reg_Dst (0 for Rt, 1 for Rd)
7 - ALU_Src (0 for Rt, 1 for immidiate)
8 - Branching_eql (1 for beq, 0 for bneq)
9 - Shift (1 for yes, 0 for no)

ALU opcode
000 : A + B //add
001 : A - B //sub
010 : A | B //or
011 : A & B //and
100 : ~(A | B) //nor
101 : B << A //sll
110 : B >>> A //srl
111 : B >> A //sra
"""

control_bits = {
    "0000": "0110100100",
    "0001": "0001000100",
    "0010": "0100000100",
    "0011": "0100000100",
    "0100": "0100001000",
    "0101": "0100001000",
    "0110": "0100000100",
    "0111": "0100001001",
    "1000": "0100001001",
    "1001": "0100001000",
    "1010": "0100001000",
    "1011": "0100000100",
    "1100": "0100001000",
    "1101": "0000010010",
    "1110": "0000010000",
    "1111": "1000000000"}
alu_opcode = {
    "0000": "000",
    "0001": "000",
    "0010": "001",
    "0011": "010",
    "0100": "000",
    "0101": "100",
    "0110": "011",
    "0111": "110",
    "1000": "101",
    "1001": "001",
    "1010": "011",
    "1011": "000",
    "1100": "010",
    "1101": "001",
    "1110": "001",
    "1111": "000"}

control_file = 'control_bits.txt'
alu_opcode_file = 'alu_opcode.txt'

with open(control_file, 'w', encoding='ascii') as f:
    f.write('v2.0 raw\n')

    for opcode, control in control_bits.items():
        line = format(int(control, 2), '03x')
        print(line)
        f.write(line+'\n')

with open(alu_opcode_file, 'w', encoding='ascii') as f:
    f.write('v2.0 raw\n')

    for opcode, aluopcode in alu_opcode.items():
        line = format(int(aluopcode, 2), '01x')
        print(line)
        f.write(line+'\n')
