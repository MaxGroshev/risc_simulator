#pragma once

#include <vector>

#include "jit_instruction_factory.hpp"

namespace jit {

class JITBasic_block {
public:
    
    JITBasic_block(JitRuntime* rt_ = nullptr) : rt(rt_) {};
    // ~JITBasic_block() {
    //     delete rt;
    // }
    
    void run() {
        
    }

    void push_back(JITInstruction instr_asm) {
        instr.push_back(instr_asm);
    }

private:
    JitRuntime* rt;
    std::vector<JITInstruction> instr;
};

}