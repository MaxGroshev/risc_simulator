#pragma once

#include "rv32i_decoder_gen.hpp"
// TODO(mgroshev): These tests can be generated as well
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

TEST_F(decoder, addi) {
    uint32_t instruction = 0x00408293;
    
    DecodedInstruction instr_decoded = RV32IDecoder::decode(instruction);
    ASSERT_TRUE(instr_decoded.name   == "addi");
    ASSERT_TRUE(instr_decoded.format == "I");
    ASSERT_TRUE(instr_decoded.funct3 == uint8_t{0});
    ASSERT_TRUE(instr_decoded.rs1 == uint8_t{1});
    ASSERT_TRUE(instr_decoded.rd == uint8_t{5});
    ASSERT_TRUE(instr_decoded.imm == uint8_t{4});
}

TEST_F(decoder, lw) {
    uint32_t instruction = 0x00412083;
    
    DecodedInstruction instr_decoded = RV32IDecoder::decode(instruction);
    ASSERT_TRUE(instr_decoded.name   == "lw");
    ASSERT_TRUE(instr_decoded.format == "I");
    ASSERT_TRUE(instr_decoded.funct3 == uint8_t{2});
    ASSERT_TRUE(instr_decoded.rs1 == uint8_t{2});
    ASSERT_TRUE(instr_decoded.rd == uint8_t{1});
    ASSERT_TRUE(instr_decoded.imm == uint8_t{4});
}

TEST_F(decoder, jalr) {
    uint32_t instruction = 0x00010067;  // jalr x0, x2, 0
    
    DecodedInstruction instr_decoded = RV32IDecoder::decode(instruction);
    ASSERT_TRUE(instr_decoded.name   == "jalr");
    ASSERT_TRUE(instr_decoded.format == "I");
    ASSERT_TRUE(instr_decoded.funct3 == uint8_t{0});
    ASSERT_TRUE(instr_decoded.rd == uint8_t{0});
    ASSERT_TRUE(instr_decoded.rs1 == uint8_t{2});
    ASSERT_TRUE(instr_decoded.imm == uint8_t{0});
}

TEST_F(decoder, sw) {
    uint32_t instruction = 0x00512423; // sw x5, 8(x2)

    DecodedInstruction instr_decoded = RV32IDecoder::decode(instruction);
    ASSERT_TRUE(instr_decoded.name   == "sw");
    ASSERT_TRUE(instr_decoded.format == "S");
    ASSERT_TRUE(instr_decoded.funct3 == uint8_t{2});
    ASSERT_TRUE(instr_decoded.rs1 == uint8_t{2});
    ASSERT_TRUE(instr_decoded.rs2 == uint8_t{5});
    ASSERT_TRUE(instr_decoded.imm == uint8_t{8});
}

TEST_F(decoder, jal) {
    uint32_t instruction = 0xff5ff06f;  // jal x0, -12

    DecodedInstruction instr_decoded = RV32IDecoder::decode(instruction);
    ASSERT_TRUE(instr_decoded.name   == "jal");
    ASSERT_TRUE(instr_decoded.format == "J");
    ASSERT_TRUE(instr_decoded.rs1 == uint8_t{0});
    ASSERT_TRUE(instr_decoded.imm == int8_t{-12});
}

TEST_F(decoder, bne) {
    uint32_t instruction =  0x00009063;  // bne

    DecodedInstruction instr_decoded = RV32IDecoder::decode(instruction);
    ASSERT_TRUE(instr_decoded.name   == "bne");
    ASSERT_TRUE(instr_decoded.format == "B");
    ASSERT_TRUE(instr_decoded.rs1 == uint8_t{1});
    ASSERT_TRUE(instr_decoded.rs2 == uint8_t{0});
}