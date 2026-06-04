"""
Assembler code to convert mips assembly code to machine code
"""

import re
import sys

instruction_opcode = {
    'lw': '0', 'sw': '1', 'subi': '2', 'ori': '3', 'add': '4', 'nor': '5', 'andi': '6', 'srl': '7',
    'sll': '8', 'sub': '9', 'and': 'a', 'addi': 'b', 'or': 'c', 'beq': 'd', 'bneq': 'e', 'j': 'f'
}


registers = {'$zero': '0', '$t0': '1', '$t1': '2', '$t2': '3',
             '$t3': '4', '$t4': '5', '$sp': '6', '$ra': '7'}


def isRInstruction(instruction):
    return instruction in ['nor', 'and', 'or', 'add', 'sub']


def isMemoryInstruction(instruction):
    return instruction in ['sw', 'lw']


def isBranchInstruction(instruction):
    return instruction in ['beq', 'bneq']


def isShiftInstruction(instruction):
    return instruction in ['srl', 'sll']


def isImmidiateInstruction(instruction):
    return instruction in ['subi', 'andi', 'addi', 'ori']


def isJumpInstruction(instruction):
    return instruction in ['j']


label_found = 0
label_dict = {}

instruction_list = []
machine_code_list = []

input_file = sys.argv[1]
output_file = sys.argv[2]

with open(input_file, 'r', encoding='ascii') as f:
    for line in f:
        without_comment = re.split(r'[#]+', line)[0].strip()
        if without_comment:
            instruction_list.append(without_comment.strip())
            print(instruction_list[-1])

for i, instruction in enumerate(instruction_list):
    if ':' in instruction:
        label = instruction.split(':')[0]
        label_dict[label] = i - label_found
        label_found += 1

for i, instruction in enumerate(instruction_list):
    if ':' in instruction:
        instruction_list.pop(i)

for i, instruction in enumerate(instruction_list):
    opcode = instruction.split(' ')[0]
    if isRInstruction(opcode):
        parts = re.split(r'[ ,]+', instruction)
        machine_code = instruction_opcode[opcode] + \
            registers[parts[2]] + registers[parts[3]] + \
            registers[parts[1]] + '0'
        machine_code_list.append(machine_code)

    elif isImmidiateInstruction(opcode):
        parts = re.split(r'[ ,]+', instruction)
        immidiate_value = int(parts[3])
        immidiate_value = (
            1 << 8) + immidiate_value if immidiate_value < 0 else immidiate_value

        machine_code = instruction_opcode[opcode] + \
            registers[parts[2]] + registers[parts[1]] + \
            format(immidiate_value, '02x')
        machine_code_list.append(machine_code)

    elif isShiftInstruction(opcode):
        parts = re.split(r'[ ,]+', instruction)
        shift_value = int(parts[3])
        shift_value = (
            1 << 4) + shift_value if shift_value < 0 else shift_value
        machine_code = instruction_opcode[opcode] + \
            registers[parts[2]] + '0'+registers[parts[1]] + \
            format(shift_value, '01x')
        machine_code_list.append(machine_code)

    elif isMemoryInstruction(opcode):
        parts = re.split(r'[ ,()]+', instruction)
        offset = int(parts[2])
        offset = (
            1 << 8) + offset if offset < 0 else offset
        machine_code = instruction_opcode[opcode] + \
            registers[parts[3]] + registers[parts[1]] + \
            format(offset, '02x')
        machine_code_list.append(machine_code)

    elif isBranchInstruction(opcode):
        parts = re.split(r'[ ,]+', instruction)
        position = label_dict[parts[3]] - i - 1
        position = (1 << 8) + position if position < 0 else position
        machine_code = instruction_opcode[opcode] + \
            registers[parts[1]] + registers[parts[2]] + \
            format(position, '02x')
        machine_code_list.append(machine_code)

    elif isJumpInstruction(opcode):
        parts = re.split(r'[ ,]+', instruction)
        position = label_dict[parts[1]]
        machine_code = instruction_opcode[opcode] + \
            format(position, '02x') + '00'
        machine_code_list.append(machine_code)


machine_code_list.append('e0000')
machine_code_list.append('e0000')
machine_code_list.append('e0000')
machine_code_list.append('e0000')

with open(output_file, 'w', encoding='ascii') as f:
    f.write('v2.0 raw\n')
    for machine_code in machine_code_list:
        f.write(machine_code + '\n')
