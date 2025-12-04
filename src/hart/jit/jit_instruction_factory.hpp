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

    void add(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr, uint64_t* regs, uint64_t* pc) {
        using namespace asmjit::a64;

        asma64->mov(x20, pc_ptr);
        asma64->ldr(x21, ptr(x20));
        
        asma64->mov(x0, regs_ptr);

        asma64->mov(x1, instr.rs1);
        asma64->lsl(x1, x1, 3);
        asma64->add(x2, x0, x1);
        asma64->ldr(x3, ptr(x2));
        
        asma64->mov(x4, instr.rs2);
        asma64->lsl(x4, x4, 3);
        asma64->add(x5, x0, x4);
        asma64->ldr(x6, ptr(x5));

        asma64->add(x7, x6, x3);

        asma64->mov(x3, regs_ptr);
        asma64->str(x7, ptr(x3, instr.rd * 8));

        asma64->add(x21, x21, 4);
        asma64->str(x21, ptr(x20));
    }

    void beq(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr, uint64_t* regs, uint64_t* pc) {
        using namespace asmjit::a64;
        
        uintptr_t instr_ptr = (uintptr_t)&instr;
        uintptr_t func_ptr  = (uintptr_t)&riscv_sim::executer::execute_beq;

        asma64->mov(x20, pc_ptr);
        asma64->ldr(x21, ptr(x20));
        
        asma64->mov(x0, instr_ptr);
        asma64->mov(x1, hart_ptr);
        asma64->mov(x2, func_ptr);
        asma64->blr(x2);

        asma64->add(x21, x21, 4);
        asma64->str(x21, ptr(x20));
    }
    void slli(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr, uint64_t* regs, uint64_t* pc) {
        using namespace asmjit::a64;

        asma64->mov(x20, pc_ptr);
        asma64->ldr(x21, ptr(x20));
        
        asma64->mov(x0, regs_ptr);
        asma64->mov(x1, instr.rs1);
        asma64->lsl(x1, x1, 3);
        asma64->add(x2, x0, x1);
        asma64->ldr(x3, ptr(x2));
        asma64->mov(x4, instr.imm);
        asma64->lsl(x5, x3, x4);

        asma64->mov(x3, regs_ptr);
        asma64->str(x5, ptr(x3, instr.rd * 8));

        asma64->add(x21, x21, 4);
        asma64->str(x21, ptr(x20));
    }
    void ld(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr, uint64_t* regs, uint64_t* pc) {
        using namespace asmjit::a64;

        asma64->mov(x20, pc_ptr);
        asma64->ldr(x21, ptr(x20));

        // get address for read
        asma64->mov(x10, regs_ptr);
        asma64->mov(x11, instr.rs1);
        asma64->lsl(x11, x11, 3);
        asma64->add(x12, x10, x11);
        asma64->ldr(x13, ptr(x12));
        asma64->mov(x15, instr.imm);
        asma64->add(x14, x15, x13);

        // run handler
        asma64->mov(x0, hart_ptr);
        asma64->mov(x1, x14);
        asma64->mov(x2, 8);
        asma64->mov(x3, memread_func_ptr);
        asma64->blr(x3);

        // store value
        asma64->mov(x4, regs_ptr);
        asma64->str(x0, ptr(x4, instr.rd * 8));

        asma64->add(x21, x21, 4);
        asma64->str(x21, ptr(x20));
    }

    void sd(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr, uint64_t* regs, uint64_t* pc) {
        using namespace asmjit::a64;

        asma64->mov(x20, pc_ptr);
        asma64->ldr(x21, ptr(x20));

        // get address for read
        asma64->mov(x10, regs_ptr);
        asma64->mov(x11, instr.rs1);
        asma64->lsl(x11, x11, 3);
        asma64->add(x12, x10, x11);
        asma64->ldr(x13, ptr(x12));
        asma64->mov(x15, instr.imm);
        asma64->add(x14, x15, x13);

        asma64->mov(x16, instr.rs2);
        asma64->lsl(x16, x16, 3);
        asma64->add(x17, x10, x16);
        asma64->ldr(x18, ptr(x17));
        
        // run handler
        asma64->mov(x0, hart_ptr);
        asma64->mov(x1, x14);
        asma64->mov(x2, x18);
        asma64->mov(x3, 8);
        asma64->mov(x4, memwrite_func_ptr);
        asma64->blr(x4);

        asma64->add(x21, x21, 4);
        asma64->str(x21, ptr(x20));
    }

public:
    JITFunctionFactory(Hart* hart) {
        mem_read_fn memread = &Hart::memory_read;
        memcpy(&memread_func_ptr, &memread, sizeof(memread_func_ptr));

        mem_write_fn memwrite = &Hart::memory_write;
        memcpy(&memwrite_func_ptr, &memwrite, sizeof(memwrite_func_ptr));

        hart_ptr = (uintptr_t)hart;
        regs_ptr = (uintptr_t)hart->get_reg_file_begin();
        pc_ptr   = (uintptr_t)hart->get_pc_ptr();
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
            case InstructionOpcode::BEQ: 
                beq(asma64, hart, instr, regs, pc); break;
            default:
                assert("unknown instr");
        }
    }
private: 
    using mem_read_fn  = uint64_t (Hart::*)(uint64_t, int) const;     //mem_access_func_signature
    using mem_write_fn = void     (Hart::*)(uint64_t, uint64_t, int); //mem_access_func_signature
    
    uintptr_t memread_func_ptr;
    uintptr_t memwrite_func_ptr;
    uintptr_t hart_ptr;
    uintptr_t regs_ptr;
    uintptr_t pc_ptr;
};
}
