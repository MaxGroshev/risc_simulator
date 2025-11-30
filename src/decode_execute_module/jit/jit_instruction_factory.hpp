#pragma once

#include "jit_instruction.hpp"
#include "../instruction_opcodes_gen.hpp"

namespace jit {

class JITFunctionFactory {
public:
    template<InstructionOpcode T>
    static std::optional<JITInstruction> compile (JitRuntime *rt, uint64_t* regs, uint64_t dst, uint64_t src1, uint64_t src2) {};

};

template<>
std::optional<JITInstruction> JITFunctionFactory::compile<InstructionOpcode::ADDI>(JitRuntime *rt, uint64_t* regs, uint64_t dst, uint64_t src1, uint64_t src2) {
    // Create JIT runtime and code holder
    CodeHolder code;
    
    code.init(rt->environment());
    a64::Assembler asma64(&code);

    asma64.mov(a64::w0, src1);
    asma64.mov(a64::w1, src2);
    asma64.mov(a64::w2, dst);

    asma64.lsl(a64::x1, a64::w0, 3);
    asma64.ldr(a64::x1, a64::ptr(a64::x0, a64::x1));

    asma64.lsl(a64::x2, a64::w1, 3);
    asma64.ldr(a64::x2, a64::ptr(a64::x0, a64::x2));

    asma64.lsl(a64::x3, a64::w2, 3);
    asma64.ldr(a64::x3, a64::ptr(a64::x0, a64::x3));

    asma64.add(a64::x4, a64::x1, a64::x2);

    asma64.str(a64::x4, a64::ptr(a64::x0, 0));

    // asma64.ret();

    void* fn;
    Error err = rt->add(&fn, &code);
    if (static_cast<int>(err) != 0) {
        std::cout << "Error: " << DebugUtils::error_as_string(err) << std::endl;
        return std::nullopt;
    }

    void (*addi)(uint64_t*) = (jit_instr)fn;
    addi(regs);

    return JITInstruction(rt, addi);
}

    // static std::optional<JITInstruction> subi(uint64_t* regs, uint64_t dst, uint64_t src1, uint64_t src2) {
    //     auto rt = std::make_unique<JitRuntime>();
    //     CodeHolder code;
        
    //     code.init(rt->environment());
    //     a64::Assembler asma64(&code);

    //     asma64.mov(a64::w0, src1);
    //     asma64.mov(a64::w1, src2);
    //     asma64.mov(a64::w2, dst);

    //     asma64.lsl(a64::x1, a64::w0, 3);
    //     asma64.ldr(a64::x1, a64::ptr(a64::x0, a64::x1));

    //     asma64.lsl(a64::x2, a64::w1, 3);
    //     asma64.ldr(a64::x2, a64::ptr(a64::x0, a64::x2));

    //     asma64.lsl(a64::x3, a64::w2, 3);
    //     asma64.ldr(a64::x3, a64::ptr(a64::x0, a64::x3));

    //     asma64.sub(a64::x4, a64::x1, a64::x2);

    //     asma64.str(a64::x4, a64::ptr(a64::x0, 0));

    //     // asma64.ret();

    //     void* fn;
    //     Error err = rt->add(&fn, &code);
    //     if (static_cast<int>(err) != 0) {
    //         std::cout << "Error: " << DebugUtils::error_as_string(err) << std::endl;
    //         return std::nullopt;
    //     }

    //     void (*subi)(uint64_t*) = (jit_instr)fn;
    //     subi(regs);

    //     // rt.release(fn);
    //     return JITInstruction(std::move(rt), subi);
    // }

    // static std::optional<JITInstruction> muli(uint64_t* regs, uint64_t dst, uint64_t src1, uint64_t src2) {
    //     auto rt = std::make_unique<JitRuntime>();
    //     CodeHolder code;
        
    //     code.init(rt->environment());
    //     a64::Assembler asma64(&code);

    //     asma64.mov(a64::w0, src1);
    //     asma64.mov(a64::w1, src2);
    //     asma64.mov(a64::w2, dst);

    //     asma64.lsl(a64::x1, a64::w0, 3);
    //     asma64.ldr(a64::x1, a64::ptr(a64::x0, a64::x1));

    //     asma64.lsl(a64::x2, a64::w1, 3);
    //     asma64.ldr(a64::x2, a64::ptr(a64::x0, a64::x2));

    //     asma64.lsl(a64::x3, a64::w2, 3);
    //     asma64.ldr(a64::x3, a64::ptr(a64::x0, a64::x3));

    //     asma64.mul(a64::x4, a64::x1, a64::x2);

    //     asma64.str(a64::x4, a64::ptr(a64::x0, 0));

    //     // asma64.ret();

    //     void* fn;
    //     Error err = rt->add(&fn, &code);
    //     if (static_cast<int>(err) != 0) {
    //         std::cout << "Error: " << DebugUtils::error_as_string(err) << std::endl;
    //         return std::nullopt;
    //     }

    //     void (*muli)(uint64_t*) = (jit_instr)fn;
    //     muli(regs);

    //     // rt.release(fn);
    //     return JITInstruction(std::move(rt), muli);
    // }
};

// }