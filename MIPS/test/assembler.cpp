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
    // Parse a single instruction and convert to machine code
    std::string parseInstruction(const std::string &instruction)
    {
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

        if (cleanInstr.empty())
        {
            return "";
        }

        // Split instruction into parts
        std::vector<std::string> parts;
        std::istringstream iss(cleanInstr);
        std::string part;
        while (std::getline(iss, part, ','))
        {
            std::istringstream partStream(part);
            while (partStream >> part)
            {
                parts.push_back(part);
            }
        }

        std::string opcode = parts[0];
        std::transform(opcode.begin(), opcode.end(), opcode.begin(), ::tolower);

        // R-type instructions
        if (r_type_opcodes.count(opcode) > 0)
        {
            std::string inst_opcode = r_type_opcodes[opcode];
            std::string rd = registers[parts[1]];
            std::string rs = registers[parts[2]];
            std::string rt = registers[parts[3]];

            string instruction = inst_opcode + rs + rt + rd + "0000";

            if (inst_opcode == "0111" || inst_opcode == "1000")
                instruction = inst_opcode + rs + "1000" + rd + decimalToBinary(stoi(parts[3]), 4);

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
                immediate = decimalToBinary(std::stoi(parts[3]), 8);
            }

            return inst_opcode + rs + rt + immediate;
        }

        return "Invalid instruction: " + cleanInstr;
    }

    // Convert entire assembly program to machine code
    std::vector<std::string> convertProgram(const std::string &assemblyCode)
    {
        std::vector<std::string> machineCode;
        std::istringstream stream(assemblyCode);
        std::string line;

        while (std::getline(stream, line))
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