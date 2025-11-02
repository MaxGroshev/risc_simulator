#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdint>

#include "instruction_opcodes_gen.hpp"

struct DecodedInstruction {
    InstructionOpcode opcode;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    int32_t imm;

    DecodedInstruction() : opcode(InstructionOpcode::UNKNOWN), rd(0), rs1(0), rs2(0), imm(0) {}
};

#endif