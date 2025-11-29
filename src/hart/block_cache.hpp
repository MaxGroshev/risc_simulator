#ifndef BLOCK_CACHE_HPP
#define BLOCK_CACHE_HPP

#include <cstdint>
#include <vector>
#include <cstddef>
#include <memory>

#include "decode_execute_module/common.hpp"

class Hart;

namespace riscv_sim {

struct Block {
    uint32_t start_pc = 0;
    bool valid = false;
    std::vector<DecodedInstruction> instrs;

    using ExecFn = void (*)(const DecodedInstruction instr, Hart& hart);
    std::vector<ExecFn> exec_fns;
};

class BlockCache {
public:
    explicit BlockCache(size_t cache_entries = 4096);

    Block* lookup(uint32_t pc);

    void install(const Block& blk);

    void invalidate_all();

    size_t capacity() const { return slots_.size(); }

private:
    size_t index_for(uint32_t start_pc) const;

    std::vector<Block> slots_;
    size_t mask_;
};

} // namespace riscv_sim

#endif // BLOCK_CACHE_HPP
