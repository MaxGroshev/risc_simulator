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
 