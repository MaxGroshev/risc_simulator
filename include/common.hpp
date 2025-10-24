#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdint>
#include <string>

struct DecodedInstruction {
    std::string name;
    std::string format;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t funct3;
    uint8_t funct7;
    int32_t imm;
    uint32_t raw_instruction;
    
    DecodedInstruction() : rd(0), rs1(0), rs2(0), funct3(0), funct7(0), imm(0), raw_instruction(0) {}
};

#endif // COMMON_HPP