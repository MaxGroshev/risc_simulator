#pragma once
#include <cstdint>
#include <string>
#include <sstream>

#include "instruction_opcodes_gen.hpp"

struct DecodedInstruction {
    InstructionOpcode opcode;
    int8_t rd;
    int8_t rs1;
    int8_t rs2;
    int32_t imm;

    DecodedInstruction() : opcode(InstructionOpcode::UNKNOWN), rd(-1), rs1(-1), rs2(-1), imm(-1) {}

    std::string to_string() const {
        std::ostringstream oss;
        oss << "{ opcode=" << static_cast<int>(opcode)
            << ", rd=" << static_cast<int>(rd)
            << ", rs1=" << static_cast<int>(rs1)
            << ", rs2=" << static_cast<int>(rs2)
            << ", imm=" << imm
            << " }";
        return oss.str();
    }
};

struct PostExecInfo {
    int32_t read_reg1; 
    int32_t read_reg2; 
    uint64_t read_reg1_val;
    uint64_t read_reg2_val;
    int32_t dest_reg; 
    uint64_t dest_reg_val;
    uint64_t imm_val;

    PostExecInfo()
      : read_reg1(-1), read_reg2(-1), read_reg1_val(0), read_reg2_val(0), dest_reg(-1), dest_reg_val(0), imm_val(0) {}

};

struct InstructionCallbackContext {
    const DecodedInstruction* instr;
    const PostExecInfo* info; 
};

struct BlockHookInfo {
    uint64_t pc;
};

using reg_t = uint64_t;
