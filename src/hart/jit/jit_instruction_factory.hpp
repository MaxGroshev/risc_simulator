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

namespace JITFunctionFactory {
    void compile(a64::Assembler* asma64, Hart* hart, DecodedInstruction& instr, uint64_t* regs, uint64_t* pc);
};

}