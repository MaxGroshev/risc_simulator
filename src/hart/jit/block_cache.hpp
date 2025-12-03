#ifndef BLOCK_CACHE_HPP
#define BLOCK_CACHE_HPP

#include <cstdint>
#include <vector>
#include <cstddef>
#include <memory>

#include "jit_basic_block.hpp"

class Hart;

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
    jit::JITBasic_block jitted_bb;
private:
    void set_jitted_bb(jit::JITBasic_block&& jitted_bb_)  { 
        jitted_bb = std::move(jitted_bb_);
        is_jitted = true;
    }

    uint64_t search_rate = 0; // show how often we want to access it
    bool is_jitted = false; 
};

class BlockCache {
public:
    explicit BlockCache(size_t cache_entries = 4096);

    Block* lookup(uint32_t pc);

    void install(Block&& blk);

    void invalidate_all();

    size_t capacity() const { return slots_.size(); }

private:
    size_t index_for(uint32_t start_pc) const;

    std::vector<Block> slots_;
    size_t mask_;
};

} // namespace riscv_sim

#endif // BLOCK_CACHE_HPP
