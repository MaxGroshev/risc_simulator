#pragma once

#include <iostream>
#include <algorithm>

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

            if((bb->search_rate == jit_bound)) {
                auto it = std::find_if(bb->instrs.begin(), bb->instrs.end(), [](DecodedInstruction& inst) {
                    return !(inst.format == InstructionFormat::J || inst.format == InstructionFormat::B);
                });
                if (it == bb->instrs.end()) {
                    compile_bb(bb);
                }
                // std::cout << "I am here" << std::endl;
                num_jit++;
            }
        }
        return bb;
    }

    // uint64_t execute(const Block* blk) {

    // }

private:
    void compile_bb(const Block* blk) {
        auto compiled_bb = jitter.compile_bb(blk->instrs, hart);
        const_cast<Block*>(blk)->set_jitted_bb(std::move(compiled_bb));// UGLY!!!
    }

    jit::JITImpl jitter;
    BlockCache   bb_cache;
    THart*       hart;
    int          num_jit = 0;

    constexpr static uint64_t jit_bound = 10;
};

}