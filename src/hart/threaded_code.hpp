#pragma once

#include <iostream>
#include <algorithm>

#include "jit/utils/lru_cache.hpp"
#include "jit/naive_cache.hpp"
#include "jit/basic_block.hpp"
#include "jit/compiler.hpp"
#include "sim_config.hpp"

namespace riscv_sim {

template<typename THart>
class ThreadedCode {
public:

    ThreadedCode(sim_config_t& sim_conf, Hart* hart_) : 
        bb_cache(sim_conf.bb_cache_size), 
        use_jit (sim_conf.use_jit), 
        jit_bound(sim_conf.jit_bound), hart(hart_) {};
   
    ThreadedCode(uint64_t bb_cache_size_, bool use_jit_, Hart* hart_) : 
        bb_cache(bb_cache_size_), use_jit(use_jit_), hart(hart_) {};
    
    void install_bb(Block&& blk) {
        bb_cache.install(blk.start_pc, std::move(blk));
    }

    bool install_bb_if_valid(Block&& blk) {
        bool valid = (blk.instrs.size() > 0);
        if (valid) {
            blk.valid = valid;
            bb_cache.install(blk.start_pc, std::move(blk));
            return true;
        }
        return false;
    }

    Block* lookup(uint32_t pc) {
        auto bb = bb_cache.lookup(pc);

        if (bb != nullptr) {
            bb->search_rate++;

            if(use_jit && (bb->search_rate == jit_bound)) {
                auto it = std::find_if(bb->instrs.begin(), bb->instrs.end(), [](DecodedInstruction& inst) {
                    return (inst.format == InstructionFormat::J || inst.format == InstructionFormat::B);
                });
                if (it == bb->instrs.end()) {
                    compile_bb(bb);
                }
            }
        }
        return bb;
    }

    bool install_and_jit(Block&& blk) {
        if (blk.instrs.empty()) {
            return false;
        }
        blk.valid = true;
        auto compiled_bb = jitter.compile_block(blk, hart);
        if (!compiled_bb) {
            return false;
        }
        blk.set_jitted_bb(std::move(compiled_bb));
        bb_cache.install(blk.start_pc, std::move(blk));
        return true;
    }

    size_t cache_capacity() const {
        return bb_cache.capacity();
    }

    bool is_jit_enabled() const {
        return use_jit;
    }

private:
    void compile_bb(const Block* blk) {
        auto compiled_bb = jitter.compile_block(*blk, hart);
        // std::cerr << "JIT: compiled BB at PC: 0x" << std::hex << blk->start_pc << std::dec << std::endl;
        const_cast<Block*>(blk)->set_jitted_bb(std::move(compiled_bb));// UGLY!!!
    }

    jit::JITImpl jitter;
    naive_cache   bb_cache;
    // utils::lru_cache<uint64_t, Block> bb_cache; //lto works with lru
    THart*       hart;
    uint64_t     jit_bound = 10;
    bool         use_jit; 
};

}