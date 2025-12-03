#pragma once

#include <cassert>
#include <iostream>
#include <memory>
#include <utility>

#include <asmjit/x86.h>
#include <asmjit/a64.h> 

#include "jit_instruction_factory.hpp"
#include "jit_basic_block.hpp"

#include "utils/cache.hpp"

class Hart;

namespace jit {

class JITImpl {
public:
    JITBasic_block compile_bb(std::vector<DecodedInstruction> instrs, Hart* hart,uint64_t* regs, uint64_t* pc) {//probably const 
        JITBasic_block bb{};

        for(auto&  instr : instrs) {
            // if (instr.format == InstructionFormat::B || instr.format == InstructionFormat::J) {
            //     throw std::runtime_error("B and J frmt instr are not supported in jit yet");
            // } else {
                jit::JITFunctionFactory::compile(bb.asma64.get(), hart, instr, regs, pc);
            // }
            std::cout << int(instr.format) << ":" << int(instr.opcode) << std::endl;
        }
        bb.add_code();
        return bb;
    }
    
private:

    // TODO(mgroshev): make typedef for label type  
    // label -> {NumOfUsages, CompiledBB \/ nullptr}(probably std::optional)
    // utils::lru_cache<uint64_t, std::pair<uint64_t, JITBasic_block>> bb_cache;
};

}