#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <bitset>

#include "../../include/common.hpp" // Include DecodedInstruction

// RV32I Decoder class
class RV32IDecoder {
public:
    static DecodedInstruction decode(uint32_t instruction);
    static std::string disassemble(const DecodedInstruction& instr);
    
private:
    // Field extraction methods
    static uint8_t get_rd(uint32_t instruction);
    static uint8_t get_rs1(uint32_t instruction);
    static uint8_t get_rs2(uint32_t instruction);
    static uint8_t get_funct3(uint32_t instruction);
    static uint8_t get_funct7(uint32_t instruction);
    static int32_t get_imm_i(uint32_t instruction);
    static int32_t get_imm_s(uint32_t instruction);
    static int32_t get_imm_b(uint32_t instruction);
    static int32_t get_imm_u(uint32_t instruction);
    static int32_t get_imm_j(uint32_t instruction);
    
    // Instruction specific decoding
    static bool decode_add(uint32_t instruction, DecodedInstruction& result);
    static bool decode_sub(uint32_t instruction, DecodedInstruction& result);
    static bool decode_sll(uint32_t instruction, DecodedInstruction& result);
    static bool decode_slt(uint32_t instruction, DecodedInstruction& result);
    static bool decode_sltu(uint32_t instruction, DecodedInstruction& result);
    static bool decode_xor(uint32_t instruction, DecodedInstruction& result);
    static bool decode_srl(uint32_t instruction, DecodedInstruction& result);
    static bool decode_sra(uint32_t instruction, DecodedInstruction& result);
    static bool decode_or(uint32_t instruction, DecodedInstruction& result);
    static bool decode_and(uint32_t instruction, DecodedInstruction& result);
    static bool decode_addi(uint32_t instruction, DecodedInstruction& result);
    static bool decode_slti(uint32_t instruction, DecodedInstruction& result);
    static bool decode_sltiu(uint32_t instruction, DecodedInstruction& result);
    static bool decode_xori(uint32_t instruction, DecodedInstruction& result);
    static bool decode_ori(uint32_t instruction, DecodedInstruction& result);
    static bool decode_andi(uint32_t instruction, DecodedInstruction& result);
    static bool decode_slli(uint32_t instruction, DecodedInstruction& result);
    static bool decode_srli(uint32_t instruction, DecodedInstruction& result);
    static bool decode_srai(uint32_t instruction, DecodedInstruction& result);
    static bool decode_lb(uint32_t instruction, DecodedInstruction& result);
    static bool decode_lh(uint32_t instruction, DecodedInstruction& result);
    static bool decode_lw(uint32_t instruction, DecodedInstruction& result);
    static bool decode_lbu(uint32_t instruction, DecodedInstruction& result);
    static bool decode_lhu(uint32_t instruction, DecodedInstruction& result);
    static bool decode_jalr(uint32_t instruction, DecodedInstruction& result);
    static bool decode_sb(uint32_t instruction, DecodedInstruction& result);
    static bool decode_sh(uint32_t instruction, DecodedInstruction& result);
    static bool decode_sw(uint32_t instruction, DecodedInstruction& result);
    static bool decode_beq(uint32_t instruction, DecodedInstruction& result);
    static bool decode_bne(uint32_t instruction, DecodedInstruction& result);
    static bool decode_blt(uint32_t instruction, DecodedInstruction& result);
    static bool decode_bge(uint32_t instruction, DecodedInstruction& result);
    static bool decode_bltu(uint32_t instruction, DecodedInstruction& result);
    static bool decode_bgeu(uint32_t instruction, DecodedInstruction& result);
    static bool decode_lui(uint32_t instruction, DecodedInstruction& result);
    static bool decode_auipc(uint32_t instruction, DecodedInstruction& result);
    static bool decode_jal(uint32_t instruction, DecodedInstruction& result);
    static bool decode_ecall(uint32_t instruction, DecodedInstruction& result);
};
