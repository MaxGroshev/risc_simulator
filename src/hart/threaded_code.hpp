#pragma once

#include <iostream>

#include "jit/block_cache.hpp"
#include "jit/compiler.hpp"

namespace riscv_sim {

template<typename THart>
class ThreadedCode {
public:
    ThreadedCode(uint64_t bb_cache_size_, Hart* hart_) : bb_cache(bb_cache_size_), hart(hart_) {};
    
    void install_bb(Block&& blk) {
        bb_cache.install(std::move(blk));
    }

    Block* lookup(uint32_t pc) {
        auto bb = bb_cache.lookup(pc);
        if (bb != nullptr) {
            bb->search_rate++;
            if(bb->search_rate == jit_bound && !one_jit) {
                compile_bb(bb);
                one_jit = true;
            }
        }
        return bb;
    }

    uint64_t execute(const Block* blk) {

    }

private:
    void compile_bb(const Block* blk) {
        auto compiled_bb = jitter.compile_bb(blk->instrs, hart, hart->get_reg_file_begin(), hart->get_pc_ptr());
        const_cast<Block*>(blk)->set_jitted_bb(std::move(compiled_bb));// UGLY!!!
    }

    jit::JITImpl jitter;
    BlockCache   bb_cache;
    THart*       hart;
    int          one_jit = false;

    constexpr static uint64_t jit_bound = 10;
};

}