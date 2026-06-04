#include <bits/stdc++.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <regex>
#include <bitset>
#include <iomanip>
using namespace std;

class MIPSAssembler
{
private:
    //   R-type instruction opcodes and function codes
    std::unordered_map<std::string, std::string> r_type_opcodes = {
        {"nor", "0101"}, {"and", "1010"}, {"or", "1100"}, {"srl", "0111"}, {"sll", "1000"}, {"add", "0100"}, {"sub", "1001"}};

    // I-type instruction opcodes
    std::unordered_map<std::string, std::string> i_type_opcodes = {
        {"subi", "0010"}, {"sw", "0001"}, {"bneq", "1110"}, {"andi", "0110"}, {"addi", "1011"}, {"ori", "0011"}, {"lw", "0000"}, {"beq", "1101"}};

    // J-type instruction opcodes
    std::unordered_map<std::string, std::string> j_type_opcodes = {
        {"j", "1111"}};

    // Register mapping
    std::unordered_map<std::string, std::string> registers = {
        {"$zero", "0000"},
        {"$t0", "0001"},
        {"$t1", "0010"},
        {"$t2", "0011"},
        {"$t3", "0100"},
        {"$t4", "0101"},
        {"$sp", "0110"},
        {"$ra", "0111"}};

    std::unordered_map<std::string, std::string> labels;
    int instruction_count = 0;
    int current_instruction_index = 0;
    std::unordered_map<std::string, int> label_positions;

    // Helper function to convert decimal to binary with specified bit length
    std::string decimalToBinary(int decimalNum, int bits)
    {
        // Handle negative numbers using two's complement
        if (decimalNum < 0)
        {
            decimalNum = (1 << bits) + decimalNum;
        }

        // Convert to binary string with leading zeros
        std::bitset<16> binary(decimalNum);
        std::string binaryStr = binary.to_string();
        return binaryStr.substr(binaryStr.length() - bits);
    }

public:
    // First pass: Collect all label positions
    void collectLabelPositions(const std::string &assemblyCode)
    {
        std::istringstream iss(assemblyCode);
        std::string line;
        int instruction_index = 0;

        while (std::getline(iss, line))
        {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            if (line.empty())
                continue;

            // Check if it's a label
            if (line.back() == ':')
            {
                std::string label = line.substr(0, line.length() - 1);
                label_positions[label] = instruction_index;
            }
            // Count non-label instructions
            else if (line.find(':') == std::string::npos)
            {
                instruction_index++;
            }
        }
    }
    // Parse a single instruction and convert to machine code
    std::string parseInstruction(const std::string &instruction)
    {
        // Remove comments and trim whitespace
        // std::string cleanInstr = instruction;
        // size_t commentPos = cleanInstr.find("#");
        // if (commentPos != std::string::npos)
        // {
        //     cleanInstr = cleanInstr.substr(0, commentPos);
        // }

        // // Trim leading and trailing whitespace
        // cleanInstr.erase(0, cleanInstr.find_first_not_of(" \t"));
        // cleanInstr.erase(cleanInstr.find_last_not_of(" \t") + 1);

        // if (cleanInstr.empty()|| cleanInstr.back() == ':')
        // {
        //     return "";
        // }

        // // Split instruction into parts
        // std::vector<std::string> parts;
        // std::istringstream iss(cleanInstr);
        // std::string part;
        // while (std::getline(iss, part, ','))
        // {
        //     std::istringstream partStream(part);
        //     while (partStream >> part)
        //     {
        //         parts.push_back(part);
        //     }
        // }

        // std::string opcode = parts[0];
        // std::transform(opcode.begin(), opcode.end(), opcode.begin(), ::tolower);

        // Remove comments and trim whitespace
        std::string cleanInstr = instruction;
        size_t commentPos = cleanInstr.find("#");
        if (commentPos != std::string::npos)
        {
            cleanInstr = cleanInstr.substr(0, commentPos);
        }

        // Trim leading and trailing whitespace
        cleanInstr.erase(0, cleanInstr.find_first_not_of(" \t"));
        cleanInstr.erase(cleanInstr.find_last_not_of(" \t") + 1);

        if (cleanInstr.empty() || cleanInstr.back() == ':')
        {
            return "";
        }

        // Split instruction into parts
        std::vector<std::string> parts;
        std::istringstream iss(cleanInstr);
        std::string part;

        // Debug: print the full clean instruction
        std::cerr << "Parsing instruction: '" << cleanInstr << "'" << std::endl;

        // Parsing with more robust splitting
        std::string opcode;
        iss >> opcode;
        parts.push_back(opcode);

        std::string remaining;
        std::getline(iss, remaining);

        // Remove leading whitespace from remaining
        remaining.erase(0, remaining.find_first_not_of(" \t"));

        // Split registers
        std::istringstream reg_iss(remaining);
        std::string reg;
        while (std::getline(reg_iss, reg, ','))
        {
            // Trim whitespace
            reg.erase(0, reg.find_first_not_of(" \t"));
            reg.erase(reg.find_last_not_of(" \t") + 1);

            if (!reg.empty())
            {
                parts.push_back(reg);
            }
        }

        // Debug: print parsed parts
        std::cerr << "Parsed parts:" << std::endl;
        for (size_t i = 0; i < parts.size(); ++i)
        {
            std::cerr << i << ": '" << parts[i] << "'" << std::endl;
        }

        std::transform(opcode.begin(), opcode.end(), opcode.begin(), ::tolower);
        // Branching instructions (beq, bneq) require special handling
        // if (opcode == "beq" || opcode == "bneq")
        // {
        //     std::string inst_opcode = i_type_opcodes[opcode];
        //     std::string rs = registers[parts[1]];
        //     std::string rt = registers[parts[2]];

        //     // Calculate branch offset
        //      std::string label = parts[3];
        //     int label_index = label_positions[label];
        //     int branch_offset = label_index - (current_instruction_index + 1);

        //     // Convert branch offset to 8-bit binary
        //     std::string immediate = decimalToBinary(branch_offset, 8);

        //     current_instruction_index++; // Increment instruction index
        //     return inst_opcode + rs + rt + immediate;
        // }
        if (opcode == "beq" || opcode == "bneq")
        {
            std::string inst_opcode = i_type_opcodes[opcode];
            std::string rs = registers[parts[1]];
            std::string rt = registers[parts[2]];

            // Calculate branch offset
            std::string label = parts[3];

            // If the label is actually a number, treat it as an immediate value
            if (std::all_of(label.begin(), label.end(), ::isdigit))
            {
                int immediate_value = std::stoi(label);
                std::string immediate = decimalToBinary(immediate_value, 8);
                current_instruction_index++; // Increment instruction index
                return inst_opcode + rs + rt + immediate;
            }

            // For label-based branching
            int label_index = label_positions[label];
            int branch_offset = label_index - (current_instruction_index + 1);

            // Convert branch offset to 8-bit binary
            std::string immediate = decimalToBinary(branch_offset, 8);

            current_instruction_index++; // Increment instruction index
            return inst_opcode + rs + rt + immediate;
        }
        // R-type instructions
        // if (r_type_opcodes.count(opcode) > 0)
        // {
        //     std::string inst_opcode = r_type_opcodes[opcode];
        //     std::string rd = registers[parts[1]];
        //     std::string rs = registers[parts[2]];
        //     std::string rt = registers[parts[3]];

        //     string instruction = inst_opcode + rs + rt + rd + "0000";

        //     if (inst_opcode == "0111" || inst_opcode == "1000")
        //         instruction = inst_opcode + rs + "1000" + rd + decimalToBinary(stoi(parts[3]), 4);

        //     return instruction;
        if (r_type_opcodes.count(opcode) > 0)
        {
            std::string inst_opcode = r_type_opcodes[opcode];

            // Ensure strict register parsing
            std::string rd_reg = parts[1];
            std::string rs_reg = parts[2];
            std::string rt_reg = parts[3];

            // Add error checking to ensure registers exist
            if (registers.find(rd_reg) == registers.end() ||
                registers.find(rs_reg) == registers.end() ||
                registers.find(rt_reg) == registers.end())
            {
                std::cerr << "Error: Invalid register in instruction" << std::endl;
                return "Invalid instruction";
            }

            std::string rd = registers[rd_reg]; // Destination register
            std::string rs = registers[rs_reg]; // First source register
            std::string rt = registers[rt_reg]; // Second source register

            // Debug print
            std::cerr << "Instruction: " << opcode
                      << ", Dest: " << rd_reg << "(" << rd << ")"
                      << ", Src1: " << rs_reg << "(" << rs << ")"
                      << ", Src2: " << rt_reg << "(" << rt << ")" << std::endl;

            // For shift instructions (srl, sll)
            if (inst_opcode == "0111" || inst_opcode == "1000")
            {
                // Special handling for shift instructions
                return inst_opcode + "0000" + rt + rd + decimalToBinary(stoi(parts[3]), 4);
            }

            // Standard R-type instruction format
            // Specifically for add: opcode + rs + rt + rd + "0000"
            std::string instruction = inst_opcode + rs + rt + rd + "0000";

            // Debug print
            std::cerr << "Generated Instruction: " << instruction << std::endl;

            return instruction;
        }
        // I-type instructions
        else if (i_type_opcodes.count(opcode) > 0)
        {
            std::string inst_opcode = i_type_opcodes[opcode];
            std::string rs, rt, immediate;

            // Memory instructions (lw, sw)
            if (opcode == "lw" || opcode == "sw")
            {
                std::regex memoryPattern(R"(\$(\w+),\s*(-?\d+)\((\$\w+)\))");
                std::smatch match;
                std::string fullMemoryPart = parts[1] + ", " + parts[2];

                if (std::regex_search(fullMemoryPart, match, memoryPattern))
                {
                    rt = registers[std::string("$") + match.str(1)];
                    rs = registers[match.str(3)];
                    immediate = decimalToBinary(std::stoi(match.str(2)), 8);
                }
                else
                {
                    return "Invalid memory instruction";
                }
            }
            // Other I-type instructions
            else
            {
                rs = registers[parts[2]];
                rt = registers[parts[1]];
                if (std::all_of(parts[3].begin(), parts[3].end(), ::isdigit))
                {
                    immediate = decimalToBinary(std::stoi(parts[3]), 8);
                }
                else
                {
                    int label_addr = std::stoi(labels[parts[3]], nullptr, 2) - instruction_count;
                    immediate = decimalToBinary(label_addr, 8);
                }
                // immediate = decimalToBinary(std::stoi(parts[3]), 8);
            }

            return inst_opcode + rs + rt + immediate;
        }
        // J-type instructions
        else if (j_type_opcodes.count(opcode))
        {
            std::string inst_opcode = j_type_opcodes[opcode];
            std::string target = labels[parts[1]];
            return inst_opcode + target + "00000000";
        }

        return "Invalid instruction: " + cleanInstr;
    }

    // Convert entire assembly program to machine code
    std::vector<std::string> convertProgram(const std::string &assemblyCode)
    { // First pass to extract labels
        instruction_count = 0;
        std::istringstream iss(assemblyCode);
        std::string line;

        while (std::getline(iss, line))
        {
            if (!line.empty() && line.back() == ':')
            {
                labels[line.substr(0, line.length() - 1)] = decimalToBinary(instruction_count, 8);
            }
            else
            {
                instruction_count++;
            }
        }
        std::vector<std::string> machineCode;
        instruction_count = 0;
        iss.clear();
        iss.str(assemblyCode);

        while (std::getline(iss, line))
        {
            std::string machineInstruction = parseInstruction(line);
            if (!machineInstruction.empty() && machineInstruction.find("Invalid") == std::string::npos)
            {
                // Convert binary to hex
                std::bitset<20> bits(machineInstruction);
                std::stringstream ss;
                ss << std::hex << std::setfill('0') << std::setw(5) << bits.to_ulong();
                machineCode.push_back(ss.str());
            }
        }
        // Add end instruction b0000 four times
        for (int i = 0; i < 4; ++i)
        {
            machineCode.push_back("e0000");
        }

        return machineCode;
    }
};

int main()
{
    MIPSAssembler assembler;
    std::string assemblyCode;
    std::ifstream inputFile("Input.txt");

    if (!inputFile)
    {
        std::cerr << "Error opening input file" << std::endl;
        return 1;
    }

    // Read entire file into string
    assemblyCode.assign((std::istreambuf_iterator<char>(inputFile)),
                        std::istreambuf_iterator<char>());

    // Convert assembly to machine code
    std::vector<std::string> machineCode = assembler.convertProgram(assemblyCode);

    // Write machine code to output file
    std::ofstream outputFile("Output.txt");
    if (!outputFile)
    {
        std::cerr << "Error opening output file" << std::endl;
        return 1;
    }

    // Write header for LogiSim compatibility
    outputFile << "v2.0 raw\n";
    for (const auto &code : machineCode)
    {
        outputFile << code << "\n";
    }

    return 0;
}