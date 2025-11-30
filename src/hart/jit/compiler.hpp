#pragma once

#include <cassert>
#include <iostream>
#include <memory>
#include <utility>

#include <asmjit/x86.h>
#include <asmjit/a64.h> 

#include "jit_instruction_factory.hpp"
#include "jit_basic_block.hpp"
class Hart;

namespace jit {

class JITImpl {
public:
    std::unique_ptr<JITBasic_block> compile_bb(std::vector<DecodedInstruction> instrs, Hart* hart) {//probably const 
        std::unique_ptr<JITBasic_block> bb = std::make_unique<JITBasic_block>();
        jit::JITFunctionFactory<Hart> factory{hart, bb->asma64.get()};
        for(auto&  instr : instrs) {
            // std::cout << int(instr.format) << ":" << int(instr.opcode) << std::endl;
            factory.compile(bb->asma64.get(), hart, instr);
        }
        bb->add_code();
        return bb;
    }
};

}