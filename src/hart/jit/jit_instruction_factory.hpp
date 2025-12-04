#pragma once

#include <iostream>
#include <cassert>
#include <memory>
#include <optional>

// #include <asmjit/x86.h>
#include <asmjit/a64.h> 

#include "decode_execute_module/instruction_opcodes_gen.hpp"
#include "decode_execute_module/executer/rv32i_executer_gen.hpp"
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
    void add(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr, uint64_t* regs, uint64_t* pc) {
        using namespace asmjit::a64;

        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));

        asma64->add(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));
        increase_pc(asma64);
    }

    void beq(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr, uint64_t* regs, uint64_t* pc) {
        using namespace asmjit::a64;
        
        uintptr_t instr_ptr = (uintptr_t)&instr;
        uintptr_t func_ptr  = (uintptr_t)&riscv_sim::executer::execute_beq;
        
        asma64->mov(x0, instr_ptr);
        asma64->mov(x1, hart_ptr);
        asma64->mov(x2, func_ptr);
        asma64->blr(x2);

        increase_pc(asma64);
    }
    void slli(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr, uint64_t* regs, uint64_t* pc) {
        using namespace asmjit::a64;
        
        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->lsl(x1, x1, x2);

        asma64->str(x1, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }
    void ld(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr, uint64_t* regs, uint64_t* pc) {
        using namespace asmjit::a64;

        // get address for read
        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->add(x1, x1, x2);

        // run handler
        asma64->mov(x0, hart_ptr);
        // asma64->mov(x1, x14);
        asma64->mov(x2, 8);
        asma64->mov(x3, memread_func_ptr);
        asma64->blr(x3);

        // store value
        asma64->str(x0, ptr(regs_beg_, instr.rd * 8));

        increase_pc(asma64);
    }

    void sd(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr, uint64_t* regs, uint64_t* pc) {
        using namespace asmjit::a64;

        // get address for read
        asma64->ldr(x1, ptr(regs_beg_, instr.rs1 * 8));
        asma64->mov(x2, instr.imm);
        asma64->add(x1, x1, x2);

        asma64->ldr(x2, ptr(regs_beg_, instr.rs2 * 8));        
        // run handler
        asma64->mov(x0, hart_ptr);

        asma64->mov(x3, 8);
        asma64->mov(x4, memwrite_func_ptr);
        asma64->blr(x4);

        increase_pc(asma64);
    }

public:
    JITFunctionFactory(Hart* hart, a64::Assembler* asma64) {
        mem_read_fn memread = &Hart::memory_read;
        memcpy(&memread_func_ptr, &memread, sizeof(memread_func_ptr));

        mem_write_fn memwrite = &Hart::memory_write;
        memcpy(&memwrite_func_ptr, &memwrite, sizeof(memwrite_func_ptr));

        hart_ptr = (uintptr_t)hart;
        regs_ptr = (uintptr_t)hart->get_reg_file_begin();
        pc_ptr   = (uintptr_t)hart->get_pc_ptr();

        asma64->mov(pc_, pc_ptr);
        asma64->mov(regs_beg_, regs_ptr);
    }
    
    void compile(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr, uint64_t* regs, uint64_t* pc) {
        assert(asma64 != nullptr);
        switch (instr.opcode) {
            case InstructionOpcode::ADD: 
                add(asma64, hart, instr, regs, pc); break;
            case InstructionOpcode::LD: 
                ld(asma64, hart, instr, regs, pc); break;
            case InstructionOpcode::SLLI: 
                slli(asma64, hart, instr, regs, pc); break;
            case InstructionOpcode::SD: 
                sd(asma64, hart, instr, regs, pc); break;
            // case InstructionOpcode::BEQ: 
            //     beq(asma64, hart, instr, regs, pc); break;
            default:
                assert("unknown instr");
        }
    }
private: 
    using mem_read_fn  = uint64_t (Hart::*)(uint64_t, int) const;     //mem_access_func_signature
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
