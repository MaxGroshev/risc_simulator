#pragma once

#include "rv32i_decoder.hpp"

//-----------------------------------------------------------------------------------------

class decoder : public ::testing::Test {
    protected:
        void SetUp() {}
};

//-----------------------------------------------------------------------------------------

TEST_F(decoder, add) {
    uint32_t instruction = 0x003100b3;
        
    DecodedInstruction instr_decoded = RV32IDecoder::decode(instruction);
    ASSERT_TRUE(instr_decoded.name   == "add");
    ASSERT_TRUE(instr_decoded.format == "R");
    ASSERT_TRUE(instr_decoded.funct3 == uint8_t{0});
    ASSERT_TRUE(instr_decoded.funct7 == uint8_t{0});
}

TEST_F(decoder, sub) {
    uint32_t instruction = 0x40310133;
    
    DecodedInstruction instr_decoded = RV32IDecoder::decode(instruction);
    ASSERT_TRUE(instr_decoded.name   == "sub");
    ASSERT_TRUE(instr_decoded.format == "R");
    ASSERT_TRUE(instr_decoded.funct3 == uint8_t{0});
    ASSERT_TRUE(instr_decoded.funct7 == uint8_t{32});
}

// uint32_t instructions[] = {
    //     0x003100b3, // add x1, x2, x3
    //     0x00408293, // addi x5, x1, 4
    //     0x00512423, // sw x5, 8(x2)
    //     0xff5ff06f  // jal x0, -12
    // };