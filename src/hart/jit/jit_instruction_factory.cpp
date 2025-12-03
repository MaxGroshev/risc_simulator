#include "jit_instruction_factory.hpp"

class Hart;
namespace jit {
namespace JITFunctionFactory {
    void add(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr, uint64_t* regs, uint64_t* pc) {
        using namespace asmjit::a64;

        uintptr_t instr_ptr = (uintptr_t)&instr;
        uintptr_t hart_ptr  = (uintptr_t)hart;
        uintptr_t func_ptr  = (uintptr_t)&riscv_sim::executer::execute_add;
        uintptr_t pc_ptr    = (uintptr_t)pc;

        // asma64->mov(x4, pc_ptr);
        // asma64->ldr(x3, ptr(x4));
        
        asma64->mov(x0, instr_ptr);
        asma64->mov(x1, hart_ptr);
        asma64->mov(x2, func_ptr);
        asma64->blr(x2);

        // asma64->add(x3, x3, 4);
        // asma64->str(x3, ptr(x4));
    }

    static void slli(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr, uint64_t* regs, uint64_t* pc) {
        using namespace asmjit::a64;

        uintptr_t instr_ptr = (uintptr_t)&instr;
        uintptr_t hart_ptr  = (uintptr_t)hart;
        uintptr_t func_ptr  = (uintptr_t)&riscv_sim::executer::execute_slli;
        uintptr_t pc_ptr    = (uintptr_t)pc;


        // asma64->mov(x4, instr_ptr);
        // asma64->ldr(x3, ptr(x4));
        
        asma64->mov(x0, instr_ptr);
        asma64->mov(x1, hart_ptr);
        asma64->mov(x2, func_ptr);
        asma64->blr(x2);

        // asma64->add(x3, x3, 4);
        // asma64->str(x3, ptr(x4));
    }

    static void beq(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr, uint64_t* regs, uint64_t* pc) {
        using namespace asmjit::a64;
        
        uintptr_t instr_ptr = (uintptr_t)&instr;
        uintptr_t hart_ptr  = (uintptr_t)hart;
        uintptr_t func_ptr  = (uintptr_t)&riscv_sim::executer::execute_beq;
        uintptr_t pc_ptr    = (uintptr_t)pc;

        // asma64->mov(x4, pc_ptr);
        // asma64->ldr(x3, ptr(x4));
        
        asma64->mov(x0, instr_ptr);
        asma64->mov(x1, hart_ptr);
        asma64->mov(x2, func_ptr);
        asma64->blr(x2);

        // asma64->add(x3, x3, 4);
        // asma64->str(x3, ptr(x4));
    }
        
    static void ld(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr, uint64_t* regs, uint64_t* pc) {
        using namespace asmjit::a64;
        
        uintptr_t instr_ptr = (uintptr_t)&instr;
        uintptr_t hart_ptr  = (uintptr_t)hart;
        uintptr_t func_ptr  = (uintptr_t)&riscv_sim::executer::execute_ld;
        uintptr_t pc_ptr    = (uintptr_t)pc;

        asma64->mov(x4, pc_ptr);
        asma64->ldr(x3, ptr(x4));
        
        asma64->mov(x0, instr_ptr);
        asma64->mov(x1, hart_ptr);

        asma64->mov(x2, func_ptr);
        asma64->blr(x2);

        asma64->add(x3, x3, 4);
        asma64->str(x3, ptr(x4));
    }

    static void sd(a64::Assembler* asma64,  Hart* hart, DecodedInstruction& instr, uint64_t* regs, uint64_t* pc) {
        using namespace asmjit::a64;
        
        uintptr_t instr_ptr = (uintptr_t)&instr;
        uintptr_t hart_ptr  = (uintptr_t)hart;
        uintptr_t func_ptr  = (uintptr_t)&riscv_sim::executer::execute_sd;
        uintptr_t pc_ptr    = (uintptr_t)pc;


        // asma64->mov(x4, pc_ptr);
        // asma64->ldr(x3, ptr(x4));
        
        asma64->mov(x0, instr_ptr);
        asma64->mov(x1, hart_ptr);
        asma64->mov(x2, func_ptr);
        asma64->blr(x2);

        // asma64->add(x3, x3, 4);
        // asma64->str(x3, ptr(x4));
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
                // asma64->mov(a64::w0, 0);
                assert("unknown instr");
        }
    }
}
}