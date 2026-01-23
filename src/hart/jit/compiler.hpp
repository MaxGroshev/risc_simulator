#pragma once

#include <cassert>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <utility>

#include <asmjit/x86.h>
#include <asmjit/a64.h> 

#include "jit_instruction_factory.hpp"
#include "jit_basic_block.hpp"
#include "basic_block.hpp"
class Hart;

namespace jit {

class JITImpl {
public:
    std::unique_ptr<JITBasic_block> compile_bb(std::vector<DecodedInstruction> instrs, Hart* hart) const {
        std::unique_ptr<JITBasic_block> bb = std::make_unique<JITBasic_block>();
#if defined(__x86_64__)
        jit::JITFunctionFactory<Hart> factory{hart, bb->asmx86.get(), &bb->exit_label};
        for(auto&  instr : instrs) {
            // std::cout << int(instr.format) << ":" << int(instr.opcode) << std::endl;
            factory.compile(bb->asmx86.get(), hart, instr);
        }
#else
        jit::JITFunctionFactory<Hart> factory{hart, bb->asma64.get()};
        for(auto&  instr : instrs) {
            // std::cout << int(instr.format) << ":" << int(instr.opcode) << std::endl;
            factory.compile(bb->asma64.get(), hart, instr);
        }
#endif
        bb->add_code();
        // bb->dump();
        return bb;
    }

    std::unique_ptr<JITBasic_block> compile_block(const riscv_sim::Block& blk, Hart* hart) const {
        if (blk.is_function_block && blk.instr_pcs.size() == blk.instrs.size()) {
            return compile_function_block(blk, hart);
        }
        return compile_bb(blk.instrs, hart);
    }
private:
    std::unique_ptr<JITBasic_block> compile_function_block(const riscv_sim::Block& blk, Hart* hart) const {
        std::unique_ptr<JITBasic_block> bb = std::make_unique<JITBasic_block>();
#if defined(__x86_64__)
        jit::JITFunctionFactory<Hart> factory{hart, bb->asmx86.get(), &bb->exit_label, true};

        std::unordered_map<uint64_t, asmjit::Label> labels;
        labels.reserve(blk.instr_pcs.size());
        for (auto pc : blk.instr_pcs) {
            labels.emplace(pc, bb->asmx86->new_label());
        }

        const size_t count = blk.instrs.size();
        for (size_t i = 0; i < count; ++i) {
            const uint64_t pc = blk.instr_pcs[i];
            bb->asmx86->bind(labels.at(pc));
            const uint64_t next_pc = pc + 4;
            const bool fallthrough_is_next = (i + 1 < count && blk.instr_pcs[i + 1] == next_pc);
            const jit::JITFunctionFactory<Hart>::X86ControlFlowContext ctx{
                pc,
                next_pc,
                fallthrough_is_next,
                &labels
            };
            factory.compile_function_x86(bb->asmx86.get(), hart, blk.instrs[i], ctx);
        }
#else
        return compile_bb(blk.instrs, hart);
#endif
        bb->add_code();
        return bb;
    }
};

}
