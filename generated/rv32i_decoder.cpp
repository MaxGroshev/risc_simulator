#include "rv32i_decoder.h"
#include <iostream>
#include <sstream>

// Field extraction implementations
uint8_t RV32IDecoder::get_rd(uint32_t instruction) {
    return (instruction >> 7) & 0x1F;
}

uint8_t RV32IDecoder::get_rs1(uint32_t instruction) {
    return (instruction >> 15) & 0x1F;
}

uint8_t RV32IDecoder::get_rs2(uint32_t instruction) {
    return (instruction >> 20) & 0x1F;
}

uint8_t RV32IDecoder::get_funct3(uint32_t instruction) {
    return (instruction >> 12) & 0x7;
}

uint8_t RV32IDecoder::get_funct7(uint32_t instruction) {
    return (instruction >> 25) & 0x7F;
}

int32_t RV32IDecoder::get_imm_i(uint32_t instruction) {
    return static_cast<int32_t>(instruction) >> 20;
}

int32_t RV32IDecoder::get_imm_s(uint32_t instruction) {
    int32_t imm = ((instruction >> 25) & 0x7F) << 5;
    imm |= ((instruction >> 7) & 0x1F);
    return static_cast<int32_t>(imm << 20) >> 20;
}

int32_t RV32IDecoder::get_imm_b(uint32_t instruction) {
    int32_t imm = ((instruction >> 31) & 0x1) << 12;
    imm |= ((instruction >> 25) & 0x3F) << 5;
    imm |= ((instruction >> 8) & 0xF) << 1;
    imm |= ((instruction >> 7) & 0x1) << 11;
    return static_cast<int32_t>(imm << 19) >> 19;
}

int32_t RV32IDecoder::get_imm_u(uint32_t instruction) {
    return instruction & 0xFFFFF000;
}

int32_t RV32IDecoder::get_imm_j(uint32_t instruction) {
    int32_t imm = ((instruction >> 31) & 0x1) << 20;
    imm |= ((instruction >> 21) & 0x3FF) << 1;
    imm |= ((instruction >> 20) & 0x1) << 11;
    imm |= ((instruction >> 12) & 0xFF) << 12;
    return static_cast<int32_t>(imm << 11) >> 11;
}

// Instruction decoding methods
bool RV32IDecoder::decode_add(uint32_t instruction, DecodedInstruction& result) {
    uint8_t opcode = instruction & 0x7F;
    uint8_t funct3 = get_funct3(instruction);
    uint8_t funct7 = get_funct7(instruction);
    
    // Check if this is the add instruction
    if (opcode == 51 && funct3 == 0 && funct7 == 0) {
        result.name = "add";
        result.format = "R";
        result.rd = get_rd(instruction);
                result.rs1 = get_rs1(instruction);
                result.rs2 = get_rs2(instruction);
                result.funct3 = get_funct3(instruction);
                result.funct7 = get_funct7(instruction);
                result.imm = 0;
        return true;
    }
    return false;
}

bool RV32IDecoder::decode_sub(uint32_t instruction, DecodedInstruction& result) {
    uint8_t opcode = instruction & 0x7F;
    uint8_t funct3 = get_funct3(instruction);
    uint8_t funct7 = get_funct7(instruction);
    
    // Check if this is the sub instruction
    if (opcode == 51 && funct3 == 0 && funct7 == 32) {
        result.name = "sub";
        result.format = "R";
        result.rd = get_rd(instruction);
                result.rs1 = get_rs1(instruction);
                result.rs2 = get_rs2(instruction);
                result.funct3 = get_funct3(instruction);
                result.funct7 = get_funct7(instruction);
                result.imm = 0;
        return true;
    }
    return false;
}


// Main decoding function
DecodedInstruction RV32IDecoder::decode(uint32_t instruction) {
    DecodedInstruction result;
    result.raw_instruction = instruction;
    
    uint8_t opcode = instruction & 0x7F;
    uint8_t funct3 = get_funct3(instruction);
    uint8_t funct7 = get_funct7(instruction);

    // Decode based on opcode and function codes
    switch (opcode) {
        case 51: // 33
            switch (get_funct3(instruction)) {
                case 0:
                    switch (get_funct7(instruction)) {
                        case 0:
                            if (decode_add(instruction, result)) return result;
                            break;
                        case 32:
                            if (decode_sub(instruction, result)) return result;
                            break;
                    }
                    break;
            }
            break;
        default:
            result.name = "UNKNOWN";
            result.format = "UNKNOWN";
    }
    
    return result;
}

// Disassembly function
std::string RV32IDecoder::disassemble(const DecodedInstruction& instr) {
    std::stringstream ss;
    ss << instr.name;
    
    if (instr.format == "R") {
        ss << " x" << static_cast<int>(instr.rd) 
           << ", x" << static_cast<int>(instr.rs1) 
           << ", x" << static_cast<int>(instr.rs2);
    } else if (instr.format == "I") {
        if (instr.name == "jalr") {
            ss << " x" << static_cast<int>(instr.rd) 
               << ", " << instr.imm 
               << "(x" << static_cast<int>(instr.rs1) << ")";
        } else if (instr.name.find("lw") != std::string::npos || 
                   instr.name.find("lh") != std::string::npos ||
                   instr.name.find("lb") != std::string::npos) {
            ss << " x" << static_cast<int>(instr.rd) 
               << ", " << instr.imm 
               << "(x" << static_cast<int>(instr.rs1) << ")";
        } else {
            ss << " x" << static_cast<int>(instr.rd) 
               << ", x" << static_cast<int>(instr.rs1) 
               << ", " << instr.imm;
        }
    } else if (instr.format == "S") {
        ss << " x" << static_cast<int>(instr.rs2) 
           << ", " << instr.imm 
           << "(x" << static_cast<int>(instr.rs1) << ")";
    } else if (instr.format == "B") {
        ss << " x" << static_cast<int>(instr.rs1) 
           << ", x" << static_cast<int>(instr.rs2) 
           << ", " << instr.imm;
    } else if (instr.format == "U") {
        ss << " x" << static_cast<int>(instr.rd) 
           << ", " << instr.imm;
    } else if (instr.format == "J") {
        ss << " x" << static_cast<int>(instr.rd) 
           << ", " << instr.imm;
    }
    
    return ss.str();
}
