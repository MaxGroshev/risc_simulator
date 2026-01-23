#pragma once

#include <iostream>
#include <cassert>
#include <memory>
#include <optional>
#include <unordered_map>

#include <asmjit/a64.h> 
#include <asmjit/x86.h> 

#include "decode_execute_module/instruction_opcodes_gen.hpp"
#include "decode_execute_module/common.hpp"

class Hart;

namespace jit {    

using namespace asmjit;

// Trampoline function declarations (defined in trampolines.cpp)
uint64_t memread_trampoline(Hart* hart, uint64_t addr, int size);
void memwrite_trampoline(Hart* hart, uint64_t addr, uint64_t value, int size);
uint64_t csrw_trampoline(Hart* hart, uint64_t csr, uint64_t value);
void ecall_trampoline(Hart* hart);
void call_trampoline(Hart* hart, uint64_t target_pc, uint64_t return_pc);

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
    JITFunctionFactory(Hart* hart, a64::Assembler* asma64, asmjit::Label* exit_label = nullptr) {
        // Use trampoline functions (plain function pointers) to avoid C++ pointer-to-member ABI issues
        memread_func_ptr = (uintptr_t)&::jit::memread_trampoline;
        memwrite_func_ptr = (uintptr_t)&::jit::memwrite_trampoline;
        csrw_func_ptr = (uintptr_t)&::jit::csrw_trampoline;
        ecall_func_ptr = (uintptr_t)&::jit::ecall_trampoline;
        call_func_ptr = (uintptr_t)&::jit::call_trampoline;
        exit_label_ = exit_label;

        hart_ptr = (uintptr_t)hart;
        regs_ptr = (uintptr_t)hart->get_reg_file_begin();
        pc_ptr   = (uintptr_t)hart->get_pc_ptr();

        asma64->mov(pc_, pc_ptr);
        asma64->mov(regs_beg_, regs_ptr);
    }

    // x86 constructor
    JITFunctionFactory(Hart* hart, asmjit::x86::Assembler* asmx86, asmjit::Label* exit_label = nullptr, bool count_instructions = false) {
        // Use trampoline functions (plain function pointers) to avoid C++ pointer-to-member ABI issues
        memread_func_ptr = (uintptr_t)&::jit::memread_trampoline;
        memwrite_func_ptr = (uintptr_t)&::jit::memwrite_trampoline;
        csrw_func_ptr = (uintptr_t)&::jit::csrw_trampoline;
        ecall_func_ptr = (uintptr_t)&::jit::ecall_trampoline;
        call_func_ptr = (uintptr_t)&::jit::call_trampoline;
        exit_label_ = exit_label;

        hart_ptr = (uintptr_t)hart;
        regs_ptr = (uintptr_t)hart->get_reg_file_begin();
        pc_ptr   = (uintptr_t)hart->get_pc_ptr();
        instr_counter_ptr = (uintptr_t)hart->get_instr_counter_ptr();
        count_instructions_ = count_instructions;

        // Move constants into chosen registers
        asmx86->mov(pc_x86_, pc_ptr);
        asmx86->mov(regs_beg_x86_, regs_ptr);
        if (count_instructions_) {
            asmx86->mov(instr_counter_x86_, instr_counter_ptr);
        }

        // Detect if paging is disabled (identity mapping) and enable direct memory access fast-path
        direct_mem_access_ = hart->is_paging_disabled();
        // std::cerr << "JIT x86: paging_disabled=" << (direct_mem_access_ ? 1 : 0) << std::endl;
        if (direct_mem_access_) {
            mem_backing_ptr_ = hart->get_memory_ptr();
            mem_backing_size_ = hart->get_memory_size();
            // Place base pointer to memory in r11 for fast addressing
            asmx86->mov(mem_base_x86_, (uint64_t)mem_backing_ptr_);
            // std::cerr << "JIT x86: Direct memory access enabled. mem_base=0x" << std::hex << (uintptr_t)mem_backing_ptr_ << std::dec << std::endl;
        }

        // std::cerr << "JIT x86: memread_func_ptr=0x" << std::hex << memread_func_ptr << " memwrite_func_ptr=0x" << memwrite_func_ptr << " hart_ptr=0x" << hart_ptr << std::dec << std::endl;
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

    // x86 compile
    void compile(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        assert(asmx86 != nullptr);
        using namespace asmjit::x86;
        emit_count_x86(asmx86);
        switch (instr.opcode) {
            // Arithmetic
            case InstructionOpcode::ADD:   add_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SUB:   sub_x86(asmx86, hart, instr); break;
            case InstructionOpcode::ADDI:  addi_x86(asmx86, hart, instr); break;
            
            // Shift operations
            case InstructionOpcode::SLLI:  slli_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SRLI:  srli_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SRAI:  srai_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SLL:   sll_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SRL:   srl_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SRA:   sra_x86(asmx86, hart, instr); break;
            
            // Bitwise logical operations
            case InstructionOpcode::XOR:   xor_x86(asmx86, hart, instr); break;
            case InstructionOpcode::XORI:  xori_x86(asmx86, hart, instr); break;
            case InstructionOpcode::OR:    or_x86(asmx86, hart, instr); break;
            case InstructionOpcode::ORI:   ori_x86(asmx86, hart, instr); break;
            case InstructionOpcode::AND:   and_x86(asmx86, hart, instr); break;
            case InstructionOpcode::ANDI:  andi_x86(asmx86, hart, instr); break;
            
            // Comparison operations
            case InstructionOpcode::SLT:   slt_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SLTI:  slti_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SLTU:  sltu_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SLTIU: sltiu_x86(asmx86, hart, instr); break;
            
            // Load operations
            case InstructionOpcode::LD:    ld_x86(asmx86, hart, instr); break;
            case InstructionOpcode::LW:    lw_x86(asmx86, hart, instr); break;
            case InstructionOpcode::LH:    lh_x86(asmx86, hart, instr); break;
            case InstructionOpcode::LB:    lb_x86(asmx86, hart, instr); break;
            case InstructionOpcode::LWU:   lwu_x86(asmx86, hart, instr); break;
            case InstructionOpcode::LHU:   lhu_x86(asmx86, hart, instr); break;
            case InstructionOpcode::LBU:   lbu_x86(asmx86, hart, instr); break;
            
            // Store operations
            case InstructionOpcode::SD:    sd_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SW:    sw_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SH:    sh_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SB:    sb_x86(asmx86, hart, instr); break;
            
            // Upper immediate
            case InstructionOpcode::LUI:   lui_x86(asmx86, hart, instr); break;
            case InstructionOpcode::AUIPC: auipc_x86(asmx86, hart, instr); break;
            
            // Jump operations
            case InstructionOpcode::JAL:   jal_x86(asmx86, hart, instr); break;
            case InstructionOpcode::JALR:  jalr_x86(asmx86, hart, instr); break;
            
            // Branch operations
            case InstructionOpcode::BEQ:   beq_x86(asmx86, hart, instr); break;
            case InstructionOpcode::BNE:   bne_x86(asmx86, hart, instr); break;
            case InstructionOpcode::BLT:   blt_x86(asmx86, hart, instr); break;
            case InstructionOpcode::BGE:   bge_x86(asmx86, hart, instr); break;
            case InstructionOpcode::BLTU:  bltu_x86(asmx86, hart, instr); break;
            case InstructionOpcode::BGEU:  bgeu_x86(asmx86, hart, instr); break;
            
            // 32-bit operations
            case InstructionOpcode::ADDIW: addiw_x86(asmx86, hart, instr); break;
            case InstructionOpcode::ADDW:  addw_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SUBW:  subw_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SLLW:  sllw_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SRLW:  srlw_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SRAW:  sraw_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SLLIW: slliw_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SRLIW: srliw_x86(asmx86, hart, instr); break;
            case InstructionOpcode::SRAIW: sraiw_x86(asmx86, hart, instr); break;
            
            // System
            case InstructionOpcode::ECALL: ecall_x86(asmx86, hart, instr); break;
            case InstructionOpcode::CSRW:  csrw_x86(asmx86, hart, instr); break;
            
            default:
                // Fallback: unknown instruction
                std::cout << "Unsupported op for x86 JIT: " << static_cast<int>(instr.opcode) << std::endl;
                assert("unknown instr for x86");
        }
    }
    
    struct X86ControlFlowContext {
        uint64_t pc;
        uint64_t next_pc;
        bool fallthrough_is_next;
        const std::unordered_map<uint64_t, asmjit::Label>* labels;
    };

    void compile_function_x86(asmjit::x86::Assembler* asmx86,
                              Hart* hart,
                              DecodedInstruction instr,
                              const X86ControlFlowContext& ctx) {
        assert(asmx86 != nullptr);
        assert(exit_label_ != nullptr);
        assert(ctx.labels != nullptr);

        const uint64_t target_pc = ctx.pc + static_cast<int64_t>(instr.imm);

        switch (instr.opcode) {
            case InstructionOpcode::BEQ:
                emit_count_x86(asmx86);
                beq_x86(asmx86, hart, instr);
                branch_dispatch_x86(asmx86, target_pc, ctx);
                return;
            case InstructionOpcode::BNE:
                emit_count_x86(asmx86);
                bne_x86(asmx86, hart, instr);
                branch_dispatch_x86(asmx86, target_pc, ctx);
                return;
            case InstructionOpcode::BLT:
                emit_count_x86(asmx86);
                blt_x86(asmx86, hart, instr);
                branch_dispatch_x86(asmx86, target_pc, ctx);
                return;
            case InstructionOpcode::BGE:
                emit_count_x86(asmx86);
                bge_x86(asmx86, hart, instr);
                branch_dispatch_x86(asmx86, target_pc, ctx);
                return;
            case InstructionOpcode::BLTU:
                emit_count_x86(asmx86);
                bltu_x86(asmx86, hart, instr);
                branch_dispatch_x86(asmx86, target_pc, ctx);
                return;
            case InstructionOpcode::BGEU:
                emit_count_x86(asmx86);
                bgeu_x86(asmx86, hart, instr);
                branch_dispatch_x86(asmx86, target_pc, ctx);
                return;
            case InstructionOpcode::JAL:
                emit_count_x86(asmx86);
                if (instr.rd == 0) {
                    jal_x86(asmx86, hart, instr);
                    auto it = ctx.labels->find(target_pc);
                    if (it != ctx.labels->end()) {
                        asmx86->jmp(it->second);
                    } else {
                        asmx86->jmp(*exit_label_);
                    }
                    return;
                }
                jal_x86(asmx86, hart, instr);
                emit_call_trampoline_x86(asmx86, target_pc, ctx.next_pc);
                emit_post_call_check_x86(asmx86, ctx);
                return;
            case InstructionOpcode::JALR:
                emit_count_x86(asmx86);
                if (is_ret_instruction(instr)) {
                    jalr_x86(asmx86, hart, instr);
                    asmx86->jmp(*exit_label_);
                    return;
                }
                if (instr.rd != 0) {
                    jalr_x86(asmx86, hart, instr);
                    emit_call_trampoline_x86(asmx86, std::nullopt, ctx.next_pc);
                    emit_post_call_check_x86(asmx86, ctx);
                    return;
                }
                jalr_x86(asmx86, hart, instr);
                asmx86->jmp(*exit_label_);
                return;
            case InstructionOpcode::ECALL:
                emit_count_x86(asmx86);
                ecall_x86(asmx86, hart, instr);
                return;
            default:
                break;
        }

        compile(asmx86, hart, instr);
        if (!ctx.fallthrough_is_next) {
            auto it = ctx.labels->find(ctx.next_pc);
            if (it != ctx.labels->end()) {
                asmx86->jmp(it->second);
            } else {
                asmx86->jmp(*exit_label_);
            }
        }
    }
private: 
    // Use plain function pointer trampolines to avoid pointer-to-member ABI complexities
    using mem_read_fn  = uint64_t (*)(Hart*, uint64_t, int);
    using mem_write_fn = void     (*)(Hart*, uint64_t, uint64_t, int);

    // AArch64 registers used in original implementation
    a64::Gp   pc_       = a64::x28;
    a64::Gp   regs_beg_ = a64::x27;

    // x86-64 registers for JIT
    asmjit::x86::Gp pc_x86_ = asmjit::x86::r13;
    asmjit::x86::Gp regs_beg_x86_ = asmjit::x86::r12;
    asmjit::x86::Gp instr_counter_x86_ = asmjit::x86::r15;

    uintptr_t memread_func_ptr;
    uintptr_t memwrite_func_ptr;
    uintptr_t csrw_func_ptr;
    uintptr_t ecall_func_ptr;
    uintptr_t call_func_ptr;
    asmjit::Label* exit_label_ = nullptr;

    // fast-path for no-paging mode
    bool direct_mem_access_ = false;
    uint8_t* mem_backing_ptr_ = nullptr;
    size_t mem_backing_size_ = 0;
    asmjit::x86::Gp mem_base_x86_ = asmjit::x86::r14; // holds base of physical memory
    uintptr_t hart_ptr;
    uintptr_t regs_ptr;
    uintptr_t pc_ptr;
    uintptr_t instr_counter_ptr = 0;
    bool count_instructions_ = false;

    bool is_ret_instruction(const DecodedInstruction& instr) const {
        return instr.opcode == InstructionOpcode::JALR &&
               instr.rd == 0 &&
               instr.rs1 == 1 &&
               instr.imm == 0;
    }

    void branch_dispatch_x86(asmjit::x86::Assembler* asmx86,
                             uint64_t target_pc,
                             const X86ControlFlowContext& ctx) {
        using namespace asmjit::x86;
        auto target_it = ctx.labels->find(target_pc);
        auto fall_it = ctx.labels->find(ctx.next_pc);
        if (target_it == ctx.labels->end() || fall_it == ctx.labels->end()) {
            asmx86->jmp(*exit_label_);
            return;
        }
        asmx86->mov(rax, ptr(pc_x86_));
        asmx86->mov(rcx, target_pc);
        asmx86->cmp(rax, rcx);
        asmx86->je(target_it->second);
        asmx86->jmp(fall_it->second);
    }

    void emit_call_trampoline_x86(asmjit::x86::Assembler* asmx86,
                                  std::optional<uint64_t> target_pc,
                                  uint64_t return_pc) {
        using namespace asmjit::x86;
        asmx86->mov(rdi, (uint64_t)hart_ptr);
        if (target_pc.has_value()) {
            asmx86->mov(rsi, target_pc.value());
        } else {
            asmx86->mov(rsi, ptr(pc_x86_));
        }
        asmx86->mov(rdx, return_pc);
        asmx86->mov(rax, (uint64_t)call_func_ptr);
        asmx86->call(rax);
    }

    void emit_post_call_check_x86(asmjit::x86::Assembler* asmx86,
                                  const X86ControlFlowContext& ctx) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(pc_x86_));
        asmx86->mov(rcx, ctx.next_pc);
        asmx86->cmp(rax, rcx);
        asmx86->jne(*exit_label_);
        if (!ctx.fallthrough_is_next) {
            auto it = ctx.labels->find(ctx.next_pc);
            if (it != ctx.labels->end()) {
                asmx86->jmp(it->second);
            } else {
                asmx86->jmp(*exit_label_);
            }
        }
    }

    void write_reg_x86(asmjit::x86::Assembler* asmx86, uint8_t rd, const asmjit::x86::Gp& value) {
        if (rd == 0) {
            return;
        }
        asmx86->mov(asmjit::x86::ptr(regs_beg_x86_, rd * 8), value);
    }

    void emit_count_x86(asmjit::x86::Assembler* asmx86) {
        if (!count_instructions_) {
            return;
        }
        asmx86->inc(asmjit::x86::qword_ptr(instr_counter_x86_));
    }

    // --- x86 implementations for common operations ---
    void increase_pc(asmjit::x86::Assembler* asmx86) {
        using namespace asmjit::x86;
        // rax will be used as a temporary here
        asmx86->mov(rax, asmjit::x86::ptr(pc_x86_));
        asmx86->add(rax, 4);
        asmx86->mov(asmjit::x86::ptr(pc_x86_), rax);
    }

    void add_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        // rax <- regs[rs1]
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        // rdx <- regs[rs2]
        asmx86->mov(rdx, ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->add(rax, rdx);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void sub_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(rdx, ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->sub(rax, rdx);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void addi_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->add(rax, (int64_t)instr.imm);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void slli_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->shl(rax, (unsigned)instr.imm);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void ld_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        // addr = regs[rs1] + imm
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        if (instr.imm != 0)
            asmx86->add(rax, (int64_t)instr.imm);

        if (direct_mem_access_) {
            // rax = physical address
            // use r10 as temp address = mem_base + rax
            asmx86->lea(r10, ptr(mem_base_x86_, rax));
            // load 8 bytes
            asmx86->mov(rax, ptr(r10));
            write_reg_x86(asmx86, instr.rd, rax);
        } else {
            // prepare call: rdi = hart_ptr, rsi = addr, rdx = size
            asmx86->mov(rdi, (uint64_t)hart_ptr);
            asmx86->mov(rsi, rax);
            asmx86->mov(rdx, 8);
            asmx86->mov(rax, (uint64_t)memread_func_ptr);
            asmx86->call(rax);
            // result in rax
            write_reg_x86(asmx86, instr.rd, rax);
        }

        increase_pc(asmx86);
    }

    void sd_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        // addr = regs[rs1] + imm
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        if (instr.imm != 0)
            asmx86->add(rax, (int64_t)instr.imm);
        // value in rdx
        asmx86->mov(rdx, ptr(regs_beg_x86_, instr.rs2 * 8));

        if (direct_mem_access_) {
            // write directly: [mem_base + addr] = value
            asmx86->lea(r10, ptr(mem_base_x86_, rax));
            asmx86->mov(ptr(r10), rdx);
        } else {
            // prepare call: rdi = hart_ptr, rsi = addr, rdx = value, rcx = size
            asmx86->mov(rdi, (uint64_t)hart_ptr);
            asmx86->mov(rsi, rax); // addr
            asmx86->mov(rcx, 8);    // size
            asmx86->mov(rax, (uint64_t)memwrite_func_ptr);
            asmx86->call(rax);
        }

        increase_pc(asmx86);
    }

    // Additional x86 implementations for missing instructions
    void sll_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(rcx, ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->shl(rax, cl); // CL register is the only valid shift count register
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void srl_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(rcx, ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->shr(rax, cl);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void sra_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(rcx, ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->sar(rax, cl);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void srli_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->shr(rax, (unsigned)instr.imm);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void srai_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->sar(rax, (unsigned)instr.imm);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void xor_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(rdx, ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->xor_(rax, rdx);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void xori_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->xor_(rax, (int64_t)instr.imm);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void or_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(rdx, ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->or_(rax, rdx);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void ori_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->or_(rax, (int64_t)instr.imm);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void and_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(rdx, ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->and_(rax, rdx);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void andi_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->and_(rax, (int64_t)instr.imm);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void slt_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(rdx, ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->cmp(rax, rdx);
        asmx86->setl(al);  // Set AL if less (signed)
        asmx86->movzx(rax, al); // Zero-extend to 64-bit
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void slti_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->cmp(rax, (int64_t)instr.imm);
        asmx86->setl(al);
        asmx86->movzx(rax, al);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void sltu_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(rdx, ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->cmp(rax, rdx);
        asmx86->setb(al);  // Set AL if below (unsigned)
        asmx86->movzx(rax, al);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void sltiu_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->cmp(rax, (uint64_t)instr.imm);
        asmx86->setb(al);
        asmx86->movzx(rax, al);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void lb_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        if (instr.imm != 0)
            asmx86->add(rax, (int64_t)instr.imm);

        if (direct_mem_access_) {
            asmx86->lea(r10, ptr(mem_base_x86_, rax));
            asmx86->movsx(rax, byte_ptr(r10));  // Sign-extend byte to 64-bit
            write_reg_x86(asmx86, instr.rd, rax);
        } else {
            asmx86->mov(rdi, (uint64_t)hart_ptr);
            asmx86->mov(rsi, rax);
            asmx86->mov(rdx, 1);
            asmx86->mov(rax, (uint64_t)memread_func_ptr);
            asmx86->call(rax);
            asmx86->movsx(rax, al);  // Sign-extend to 64-bit
            write_reg_x86(asmx86, instr.rd, rax);
        }

        increase_pc(asmx86);
    }

    void lh_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        if (instr.imm != 0)
            asmx86->add(rax, (int64_t)instr.imm);

        if (direct_mem_access_) {
            asmx86->lea(r10, ptr(mem_base_x86_, rax));
            asmx86->movsx(rax, word_ptr(r10));  // Sign-extend word to 64-bit
            write_reg_x86(asmx86, instr.rd, rax);
        } else {
            asmx86->mov(rdi, (uint64_t)hart_ptr);
            asmx86->mov(rsi, rax);
            asmx86->mov(rdx, 2);
            asmx86->mov(rax, (uint64_t)memread_func_ptr);
            asmx86->call(rax);
            asmx86->movsx(rax, ax);  // Sign-extend to 64-bit
            write_reg_x86(asmx86, instr.rd, rax);
        }

        increase_pc(asmx86);
    }

    void lw_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        if (instr.imm != 0)
            asmx86->add(rax, (int64_t)instr.imm);

        if (direct_mem_access_) {
            asmx86->lea(r10, ptr(mem_base_x86_, rax));
            asmx86->movsxd(rax, dword_ptr(r10));  // Sign-extend dword to 64-bit
            write_reg_x86(asmx86, instr.rd, rax);
        } else {
            asmx86->mov(rdi, (uint64_t)hart_ptr);
            asmx86->mov(rsi, rax);
            asmx86->mov(rdx, 4);
            asmx86->mov(rax, (uint64_t)memread_func_ptr);
            asmx86->call(rax);
            asmx86->movsxd(rax, eax);  // Sign-extend to 64-bit
            write_reg_x86(asmx86, instr.rd, rax);
        }

        increase_pc(asmx86);
    }

    void lbu_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        if (instr.imm != 0)
            asmx86->add(rax, (int64_t)instr.imm);

        if (direct_mem_access_) {
            asmx86->lea(r10, ptr(mem_base_x86_, rax));
            asmx86->movzx(rax, byte_ptr(r10));  // Zero-extend byte to 64-bit
            write_reg_x86(asmx86, instr.rd, rax);
        } else {
            asmx86->mov(rdi, (uint64_t)hart_ptr);
            asmx86->mov(rsi, rax);
            asmx86->mov(rdx, 1);
            asmx86->mov(rax, (uint64_t)memread_func_ptr);
            asmx86->call(rax);
            asmx86->movzx(rax, al);  // Zero-extend to 64-bit
            write_reg_x86(asmx86, instr.rd, rax);
        }

        increase_pc(asmx86);
    }

    void lhu_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        if (instr.imm != 0)
            asmx86->add(rax, (int64_t)instr.imm);

        if (direct_mem_access_) {
            asmx86->lea(r10, ptr(mem_base_x86_, rax));
            asmx86->movzx(rax, word_ptr(r10));  // Zero-extend word to 64-bit
            write_reg_x86(asmx86, instr.rd, rax);
        } else {
            asmx86->mov(rdi, (uint64_t)hart_ptr);
            asmx86->mov(rsi, rax);
            asmx86->mov(rdx, 2);
            asmx86->mov(rax, (uint64_t)memread_func_ptr);
            asmx86->call(rax);
            asmx86->movzx(rax, ax);  // Zero-extend to 64-bit
            write_reg_x86(asmx86, instr.rd, rax);
        }

        increase_pc(asmx86);
    }

    void lwu_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        if (instr.imm != 0)
            asmx86->add(rax, (int64_t)instr.imm);

        if (direct_mem_access_) {
            asmx86->lea(r10, ptr(mem_base_x86_, rax));
            asmx86->mov(eax, dword_ptr(r10));  // Load 32-bit (zero-extends upper 32 bits)
            write_reg_x86(asmx86, instr.rd, rax);
        } else {
            asmx86->mov(rdi, (uint64_t)hart_ptr);
            asmx86->mov(rsi, rax);
            asmx86->mov(rdx, 4);
            asmx86->mov(rax, (uint64_t)memread_func_ptr);
            asmx86->call(rax);
            asmx86->mov(eax, eax);  // Zero-extend to 64-bit
            write_reg_x86(asmx86, instr.rd, rax);
        }

        increase_pc(asmx86);
    }

    void sb_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        if (instr.imm != 0)
            asmx86->add(rax, (int64_t)instr.imm);
        asmx86->mov(rdx, ptr(regs_beg_x86_, instr.rs2 * 8));

        if (direct_mem_access_) {
            asmx86->lea(r10, ptr(mem_base_x86_, rax));
            asmx86->mov(byte_ptr(r10), dl);  // Store low byte
        } else {
            asmx86->mov(rdi, (uint64_t)hart_ptr);
            asmx86->mov(rsi, rax);
            asmx86->mov(rcx, 1);
            asmx86->mov(rax, (uint64_t)memwrite_func_ptr);
            asmx86->call(rax);
        }

        increase_pc(asmx86);
    }

    void sh_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        if (instr.imm != 0)
            asmx86->add(rax, (int64_t)instr.imm);
        asmx86->mov(rdx, ptr(regs_beg_x86_, instr.rs2 * 8));

        if (direct_mem_access_) {
            asmx86->lea(r10, ptr(mem_base_x86_, rax));
            asmx86->mov(word_ptr(r10), dx);  // Store low word
        } else {
            asmx86->mov(rdi, (uint64_t)hart_ptr);
            asmx86->mov(rsi, rax);
            asmx86->mov(rcx, 2);
            asmx86->mov(rax, (uint64_t)memwrite_func_ptr);
            asmx86->call(rax);
        }

        increase_pc(asmx86);
    }

    void sw_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        if (instr.imm != 0)
            asmx86->add(rax, (int64_t)instr.imm);
        asmx86->mov(rdx, ptr(regs_beg_x86_, instr.rs2 * 8));

        if (direct_mem_access_) {
            asmx86->lea(r10, ptr(mem_base_x86_, rax));
            asmx86->mov(dword_ptr(r10), edx);  // Store low dword
        } else {
            asmx86->mov(rdi, (uint64_t)hart_ptr);
            asmx86->mov(rsi, rax);
            asmx86->mov(rcx, 4);
            asmx86->mov(rax, (uint64_t)memwrite_func_ptr);
            asmx86->call(rax);
        }

        increase_pc(asmx86);
    }

    void lui_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, (int64_t)instr.imm << 12);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void auipc_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(pc_x86_));
        asmx86->add(rax, ((int64_t)instr.imm << 12));
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void jal_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(pc_x86_));
        asmx86->add(rax, 4);
        write_reg_x86(asmx86, instr.rd, rax);  // Store return address
        asmx86->mov(rax, ptr(pc_x86_));
        asmx86->add(rax, (int64_t)instr.imm);
        asmx86->mov(ptr(pc_x86_), rax);  // Set PC to new address
    }

    void jalr_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(pc_x86_));
        asmx86->add(rax, 4);
        write_reg_x86(asmx86, instr.rd, rax);  // Store return address
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->add(rax, (int64_t)instr.imm);
        asmx86->and_(rax, (int64_t)-2);
        asmx86->mov(ptr(pc_x86_), rax);  // Set PC to rs1 + imm
    }

    void beq_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(pc_x86_));
        asmx86->mov(rcx, rax);
        asmx86->add(rax, (int64_t)instr.imm);
        asmx86->add(rcx, 4);

        asmx86->mov(rdx, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(r8, ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->cmp(rdx, r8);

        asmx86->cmovne(rax, rcx); // If not equal, use fall-through
        asmx86->mov(ptr(pc_x86_), rax);
    }

    void bne_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(pc_x86_));
        asmx86->mov(rcx, rax);
        asmx86->add(rax, (int64_t)instr.imm);
        asmx86->add(rcx, 4);

        asmx86->mov(rdx, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(r8, ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->cmp(rdx, r8);

        asmx86->cmove(rax, rcx);   // If equal, use fall-through
        asmx86->mov(ptr(pc_x86_), rax);
    }

    void blt_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(pc_x86_));
        asmx86->mov(rcx, rax);
        asmx86->add(rax, (int64_t)instr.imm);
        asmx86->add(rcx, 4);

        asmx86->mov(rdx, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(r8, ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->cmp(rdx, r8);

        asmx86->cmovge(rax, rcx);  // If greater/equal, use fall-through
        asmx86->mov(ptr(pc_x86_), rax);
    }

    void bge_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(pc_x86_));
        asmx86->mov(rcx, rax);
        asmx86->add(rax, (int64_t)instr.imm);
        asmx86->add(rcx, 4);

        asmx86->mov(rdx, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(r8, ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->cmp(rdx, r8);

        asmx86->cmovl(rax, rcx);   // If less, use fall-through
        asmx86->mov(ptr(pc_x86_), rax);
    }

    void bltu_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(pc_x86_));
        asmx86->mov(rcx, rax);
        asmx86->add(rax, (int64_t)instr.imm);
        asmx86->add(rcx, 4);

        asmx86->mov(rdx, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(r8, ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->cmp(rdx, r8);

        asmx86->cmovae(rax, rcx);  // If above/equal (unsigned), use fall-through
        asmx86->mov(ptr(pc_x86_), rax);
    }

    void bgeu_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(pc_x86_));
        asmx86->mov(rcx, rax);
        asmx86->add(rax, (int64_t)instr.imm);
        asmx86->add(rcx, 4);

        asmx86->mov(rdx, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(r8, ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->cmp(rdx, r8);

        asmx86->cmovb(rax, rcx);   // If below (unsigned), use fall-through
        asmx86->mov(ptr(pc_x86_), rax);
    }

    void addiw_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(eax, dword_ptr(regs_beg_x86_, instr.rs1 * 8));  // Load 32-bit value
        asmx86->add(eax, (int32_t)instr.imm);
        asmx86->movsxd(rax, eax);  // Sign-extend to 64-bit
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void addw_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(eax, dword_ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(edx, dword_ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->add(eax, edx);
        asmx86->movsxd(rax, eax);  // Sign-extend to 64-bit
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void subw_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(eax, dword_ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(edx, dword_ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->sub(eax, edx);
        asmx86->movsxd(rax, eax);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void sllw_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(eax, dword_ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(ecx, dword_ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->shl(eax, cl);
        asmx86->movsxd(rax, eax);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void srlw_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(eax, dword_ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(ecx, dword_ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->shr(eax, cl);
        write_reg_x86(asmx86, instr.rd, rax);  // Already zero-extended
        increase_pc(asmx86);
    }

    void sraw_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(eax, dword_ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(ecx, dword_ptr(regs_beg_x86_, instr.rs2 * 8));
        asmx86->sar(eax, cl);
        asmx86->movsxd(rax, eax);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void slliw_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(eax, dword_ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->shl(eax, (unsigned)instr.imm);
        asmx86->movsxd(rax, eax);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void srliw_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(eax, dword_ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->shr(eax, (unsigned)instr.imm);
        write_reg_x86(asmx86, instr.rd, rax);  // Zero-extended by default
        increase_pc(asmx86);
    }

    void sraiw_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(eax, dword_ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->sar(eax, (unsigned)instr.imm);
        asmx86->movsxd(rax, eax);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }

    void ecall_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        increase_pc(asmx86);
        asmx86->mov(rdi, (uint64_t)hart_ptr);
        asmx86->mov(rax, (uint64_t)ecall_func_ptr);
        asmx86->call(rax);
        asmx86->jmp(*exit_label_);
    }

    void csrw_x86(asmjit::x86::Assembler* asmx86, Hart* hart, DecodedInstruction& instr) {
        using namespace asmjit::x86;
        asmx86->mov(rax, ptr(regs_beg_x86_, instr.rs1 * 8));
        asmx86->mov(rdi, (uint64_t)hart_ptr);
        asmx86->mov(rsi, (uint64_t)instr.imm);
        asmx86->mov(rdx, rax);
        asmx86->mov(rax, (uint64_t)csrw_func_ptr);
        asmx86->call(rax);
        write_reg_x86(asmx86, instr.rd, rax);
        increase_pc(asmx86);
    }
};
}
