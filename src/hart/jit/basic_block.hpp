#pragma once

#include <cstdint>
#include <vector>
#include <cstddef>
#include <memory>

#include "jit_basic_block.hpp"

namespace riscv_sim {

template<typename THart>
class ThreadedCode;

class Block {

template<typename THart>
friend class ThreadedCode;

public:
    uint32_t start_pc = 0;
    bool     valid    = false;
    std::vector<DecodedInstruction> instrs;

    using ExecFn = void (*)(const DecodedInstruction &instr, Hart& hart);
    std::vector<ExecFn> exec_fns;

    bool get_is_jitted() const { return is_jitted;}
    std::unique_ptr<jit::JITBasic_block> jitted_bb;
private:
    void set_jitted_bb(std::unique_ptr<jit::JITBasic_block> jitted_bb_)  { 
        jitted_bb = std::move(jitted_bb_);
        is_jitted = true;
    }

    uint64_t search_rate = 0; // show how often we want to access it
    bool is_jitted = false; 
};

} // namespace riscv_sim
