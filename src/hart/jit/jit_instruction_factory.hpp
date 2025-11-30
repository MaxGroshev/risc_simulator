#pragma once

#include <iostream>
#include <cassert>
#include <memory>
#include <optional>

#include <asmjit/a64.h> 

#include "decode_execute_module/instruction_opcodes_gen.hpp"
#include "decode_execute_module/common.hpp"

class Hart;

namespace jit {    

using namespace asmjit;

template<class Hart>
class JITFunctionFactory {
    void increase_pc(a64::Assembler* asma64) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(pc_));
        asma64->add(x1, x1, 4);
        asma64->str(x1, ptr(pc_));
    }


    void slt(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));

        asma64->cmp(x1, x2);
        asma64->cset(x1, CondCode::kLT);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));
        increase_pc(asma64);
    }

    void slti(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);

        asma64->cmp(x1, x2);
        asma64->cset(x1, CondCode::kLT);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));
        increase_pc(asma64);
    }

    void sltiu(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);

        asma64->cmp(x1, x2);
        asma64->cset(x1, CondCode::kLO);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));
        increase_pc(asma64);
    }

    void sltu(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));

        asma64->cmp(x1, x2);
        asma64->cset(x1, CondCode::kLO);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));
        increase_pc(asma64);
    }

    void xor_(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));

        asma64->eor(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));
        increase_pc(asma64);
    }

    void xori(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);

        asma64->eor(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));
        increase_pc(asma64);
    }

    void or_(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));

        asma64->orn(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));
        increase_pc(asma64);
    }

    void ori(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);

        asma64->orn(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));
        increase_pc(asma64);
    }

    void and_(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));

        asma64->and_(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));
        increase_pc(asma64);
    }

    void andi(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);

        asma64->and_(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));
        increase_pc(asma64);
    }

    void add(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));

        asma64->add(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));
        increase_pc(asma64);
    }

    void addw(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->and_(x1, x1, 0xFFFFFFFFULL);
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));
        asma64->and_(x2, x2, 0xFFFFFFFFULL);

        asma64->add(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));
        increase_pc(asma64);
    }

    void addiw(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->and_(x1, x1, 0xFFFFFFFFULL);
        asma64->mov(x2, instr.imm);
        asma64->and_(x2, x2, 0xFFFFFFFFULL);

        asma64->add(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));
        increase_pc(asma64);
    }

    void addi(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);

        asma64->add(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));
        increase_pc(asma64);
    }

    void sub(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));

        asma64->sub(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));
        increase_pc(asma64);
    }

    void subw(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->and_(x1, x1, 0xFFFFFFFFULL);
        asma64->mov(x2, instr.imm);
        asma64->and_(x2, x2, 0xFFFFFFFFULL);

        asma64->sub(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));
        increase_pc(asma64);
    }

    void slli(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;
        
        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->lsl(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void lui(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;
        
        asma64->mov(x1, instr.imm);
        asma64->mov(x2, 12U);
        asma64->lsl(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void auipc(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;
        
        asma64->mov(x1, instr.imm);
        asma64->mov(x2, 12U);
        asma64->lsl(x1, x1, x2);

        asma64->ldr(x2, ptr(pc_));
        asma64->add(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void srli(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;
        
        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->lsr(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void srai(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;
        
        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->asr(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void slliw(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;
        
        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->and_(x1, x1, 0xFFFFFFFFULL);
        asma64->mov(x2, instr.imm);
        asma64->and_(x2, x2, 0xFFFFFFFFULL);
        asma64->lsl(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void srliw(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;
        
        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->and_(x1, x1, 0xFFFFFFFFULL);
        asma64->mov(x2, instr.imm);
        asma64->and_(x2, x2, 31U);
        asma64->lsr(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void srl(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;
        
        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->lsr(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void sra(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;
        
        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->asr(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void sraiw(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;
        
        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->and_(x1, x1, 0xFFFFFFFFULL);
        asma64->mov(x2, instr.imm);
        asma64->and_(x2, x2, 31U);
        asma64->asr(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void sll(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;
        
        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));
        asma64->lsl(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void sllw(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;
        
        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->and_(x1, x1, 0xFFFFFFFFULL);
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));
        asma64->and_(x2, x2, 0xFFFFFFFFULL);
        asma64->lsl(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void srlw(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;
        
        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->and_(x1, x1, 0xFFFFFFFFULL);
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));
        asma64->and_(x2, x2, 0xFFFFFFFFULL);
        asma64->lsr(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void sraw(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;
        
        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->and_(x1, x1, 0xFFFFFFFFULL);
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));
        asma64->and_(x2, x2, 31U);
        asma64->asr(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void ld(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->add(x1, x1, x2);
        asma64->mov(x0, hart_ptr);
        asma64->mov(x2, 8);
        asma64->mov(x3, memread_func_ptr);
        asma64->blr(x3);
        asma64->str(x0, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void lb(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->add(x1, x1, x2);
        asma64->mov(x0, hart_ptr);
        asma64->mov(x2, 1);
        asma64->mov(x3, memread_func_ptr);
        asma64->blr(x3);
        asma64->str(x0, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void lh(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->add(x1, x1, x2);
        asma64->mov(x0, hart_ptr);
        asma64->mov(x2, 2);
        asma64->mov(x3, memread_func_ptr);
        asma64->blr(x3);
        asma64->str(x0, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void do_ecall(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;
        using ecall_fn  = void (Hart::*)();     //mem_access_func_signature

        uintptr_t ecall_func_ptp;
        ecall_fn ecall_func = &Hart::do_ecall;
        memcpy(&ecall_func_ptp, &ecall_func, sizeof(ecall_func_ptp));

        asma64->mov(x0, hart_ptr);
        asma64->mov(x2, ecall_func_ptp);
        asma64->blr(x2);

        increase_pc(asma64);
    }

    void lw(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->add(x1, x1, x2);
        asma64->mov(x0, hart_ptr);
        asma64->mov(x2, 4);
        asma64->mov(x3, memread_func_ptr);
        asma64->blr(x3);
        asma64->str(x0, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void lbu(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->add(x1, x1, x2);
        asma64->mov(x0, hart_ptr);
        asma64->mov(x2, 1);
        asma64->mov(x3, memread_func_ptr);
        asma64->blr(x3);
        asma64->str(x0, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void lhu(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->add(x1, x1, x2);
        asma64->mov(x0, hart_ptr);
        asma64->mov(x2, 2);
        asma64->mov(x3, memread_func_ptr);
        asma64->blr(x3);
        asma64->str(x0, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void lwu(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->add(x1, x1, x2);
        asma64->mov(x0, hart_ptr);
        asma64->mov(x2, 4);
        asma64->mov(x3, memread_func_ptr);
        asma64->blr(x3);
        asma64->str(x0, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void sd(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->add(x1, x1, x2);

        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));        
        asma64->mov(x0, hart_ptr);

        asma64->mov(x3, 8);
        asma64->mov(x4, memwrite_func_ptr);
        asma64->blr(x4);

        increase_pc(asma64);
    }

    void sb(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->add(x1, x1, x2);

        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));        
        asma64->mov(x0, hart_ptr);

        asma64->mov(x3, 1);
        asma64->mov(x4, memwrite_func_ptr);
        asma64->blr(x4);

        increase_pc(asma64);
    }

    void sh(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->add(x1, x1, x2);

        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));        
        asma64->mov(x0, hart_ptr);

        asma64->mov(x3, 2);
        asma64->mov(x4, memwrite_func_ptr);
        asma64->blr(x4);

        increase_pc(asma64);
    }

    void sw(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->add(x1, x1, x2);

        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));        
        asma64->mov(x0, hart_ptr);

        asma64->mov(x3, 4);
        asma64->mov(x4, memwrite_func_ptr);
        asma64->blr(x4);

        increase_pc(asma64);
    }

    void jalr(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));

        asma64->ldr(x3, ptr(pc_));
        asma64->add(x2, x3, 4);
        asma64->str(x2, ptr(regs_beg_, instr.rd * 8));
        
        asma64->add(x2, x1, x3);
        asma64->str(x1, ptr(pc_));
    }

    void jal(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->mov(x1, instr.imm);

        asma64->ldr(x3, ptr(pc_));
        asma64->add(x2, x3, 4);
        asma64->str(x2, ptr(regs_beg_, instr.rd * 8));
        
        asma64->add(x2, x1, x3);
        asma64->str(x1, ptr(pc_));
    }


    void beq(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;
        
        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));
        
        asma64->ldr(x3, ptr(pc_));
        asma64->add(x4, x3, instr.imm);
        asma64->add(x3, x3, 4);

        asma64->cmp(x1, x2);
        asma64->csel(x1, x4, x3, CondCode::kEQ);
        
        asma64->str(x1, ptr(pc_));
    }

    void bne(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));
        
        asma64->ldr(x3, ptr(pc_));
        asma64->add(x4, x3, instr.imm);
        asma64->add(x3, x3, 4);

        asma64->cmp(x1, x2);
        asma64->csel(x1, x4, x3, CondCode::kNE);
        
        asma64->str(x1, ptr(pc_));
    }

    void blt(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));
        
        asma64->ldr(x3, ptr(pc_));
        asma64->add(x4, x3, instr.imm);
        asma64->add(x3, x3, 4);

        asma64->cmp(x1, x2);
        asma64->csel(x1, x4, x3, CondCode::kLT);
        
        asma64->str(x1, ptr(pc_));
    }

    void bltu(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));
        
        asma64->ldr(x3, ptr(pc_));
        asma64->add(x4, x3, instr.imm);
        asma64->add(x3, x3, 4);

        asma64->cmp(x1, x2);
        asma64->csel(x1, x4, x3, CondCode::kUnsignedLT);
        
        asma64->str(x1, ptr(pc_));
    }

    void bge(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));
        
        asma64->ldr(x3, ptr(pc_));
        asma64->add(x4, x3, instr.imm);
        asma64->add(x3, x3, 4);

        asma64->cmp(x1, x2);
        asma64->csel(x1, x4, x3, CondCode::kGE);
        
        asma64->str(x1, ptr(pc_));
    }

    void bgeu(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));
        
        asma64->ldr(x3, ptr(pc_));
        asma64->add(x4, x3, instr.imm);
        asma64->add(x3, x3, 4);

        asma64->cmp(x1, x2);
        asma64->csel(x1, x4, x3, CondCode::kUnsignedGE);
        
        asma64->str(x1, ptr(pc_));
    }

public:
    JITFunctionFactory(Hart* hart, a64::Assembler* asma64) {
        mem_read_fn memread = &Hart::load;
        memcpy(&memread_func_ptr, &memread, sizeof(memread_func_ptr));

        mem_write_fn memwrite = &Hart::store;
        memcpy(&memwrite_func_ptr, &memwrite, sizeof(memwrite_func_ptr));

        hart_ptr = (uintptr_t)hart;
        regs_ptr = (uintptr_t)hart->get_reg_file_begin();
        pc_ptr   = (uintptr_t)hart->get_pc_ptr();

        asma64->mov(pc_, pc_ptr);
        asma64->mov(regs_beg_, regs_ptr);
    }
    
    void compile(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr) {
        assert(asma64 != nullptr);
        switch (instr.opcode) {
            case InstructionOpcode::ADD:   add(asma64, hart, instr); break;
            case InstructionOpcode::SUB:   sub(asma64, hart, instr); break;
            case InstructionOpcode::ADDI:  addi(asma64, hart, instr); break;
            case InstructionOpcode::LD:    ld(asma64, hart, instr); break;
            case InstructionOpcode::SLLI:  slli(asma64, hart, instr); break;
            case InstructionOpcode::SD:    sd(asma64, hart, instr); break;
            case InstructionOpcode::SLL:   sll(asma64, hart, instr); break;
            case InstructionOpcode::LB:    lb(asma64, hart, instr); break;
            case InstructionOpcode::LH:    lh(asma64, hart, instr); break;
            case InstructionOpcode::LW:    lw(asma64, hart, instr); break;
            case InstructionOpcode::LBU:   lbu(asma64, hart, instr); break;
            case InstructionOpcode::LHU:   lhu(asma64, hart, instr); break;
            case InstructionOpcode::LWU:   lwu(asma64, hart, instr); break;
            case InstructionOpcode::SB:    sb(asma64, hart, instr); break;
            case InstructionOpcode::SH:    sh(asma64, hart, instr); break;
            case InstructionOpcode::SW:    sw(asma64, hart, instr); break;
            case InstructionOpcode::ADDIW: addiw(asma64, hart, instr); break;
            case InstructionOpcode::SLLIW: slliw(asma64, hart, instr); break;
            case InstructionOpcode::SRLIW: srliw(asma64, hart, instr); break;
            case InstructionOpcode::SRAIW: sraiw(asma64, hart, instr); break;
            case InstructionOpcode::ADDW:  addw(asma64, hart, instr); break;
            case InstructionOpcode::SUBW:  subw(asma64, hart, instr); break;
            case InstructionOpcode::SLLW:  sllw(asma64, hart, instr); break;
            case InstructionOpcode::SRLW:  srlw(asma64, hart, instr); break;
            case InstructionOpcode::SLT:   slt(asma64, hart, instr); break;
            case InstructionOpcode::SLTU:  sltu(asma64, hart, instr); break;
            case InstructionOpcode::XOR:   xor_(asma64, hart, instr); break;
            case InstructionOpcode::SRL:   srl(asma64, hart, instr); break;
            case InstructionOpcode::SRA:   sra(asma64, hart, instr); break;
            case InstructionOpcode::OR:    or_(asma64, hart, instr); break;
            case InstructionOpcode::AND:   and_(asma64, hart, instr); break;
            case InstructionOpcode::SLTI:  slti(asma64, hart, instr); break;
            case InstructionOpcode::SLTIU: sltiu(asma64, hart, instr); break;
            case InstructionOpcode::XORI:  xori(asma64, hart, instr); break;
            case InstructionOpcode::ORI:   ori(asma64, hart, instr); break;
            case InstructionOpcode::ANDI:  andi(asma64, hart, instr); break;
            case InstructionOpcode::SRLI:  srli(asma64, hart, instr); break;
            case InstructionOpcode::JALR:  jalr(asma64, hart, instr); break;
            case InstructionOpcode::BEQ:   beq(asma64, hart, instr); break;
            case InstructionOpcode::BNE:   bne(asma64, hart, instr); break;
            case InstructionOpcode::BLT:   blt(asma64, hart, instr); break;
            case InstructionOpcode::BGE:   bge(asma64, hart, instr); break;
            case InstructionOpcode::BLTU:  bltu(asma64, hart, instr); break;
            case InstructionOpcode::BGEU:  bgeu(asma64, hart, instr); break;
            case InstructionOpcode::LUI:   lui(asma64, hart, instr); break;
            case InstructionOpcode::AUIPC: auipc(asma64, hart, instr); break;
            case InstructionOpcode::JAL:   jal(asma64, hart, instr); break;
            case InstructionOpcode::ECALL: do_ecall(asma64, hart, instr); break;
            default:
                std::cout << "Nowhere" << std::endl;
                assert("unknown instr");
        }
    }
private: 
    using mem_read_fn  = uint64_t (Hart::*)(uint64_t, int);     //mem_access_func_signature
    using mem_write_fn = void     (Hart::*)(uint64_t, uint64_t, int); //mem_access_func_signature
    a64::Gp   pc_       = a64::x28;
    a64::Gp   regs_beg_ = a64::x27;
    uintptr_t memread_func_ptr;
    uintptr_t memwrite_func_ptr;
    uintptr_t hart_ptr;
    uintptr_t regs_ptr;
    uintptr_t pc_ptr;
};
}
