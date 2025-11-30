#pragma once

#include <cstdint>
#include <vector>
#include <cstddef>
#include <memory>

#include "basic_block.hpp"

namespace riscv_sim {

class naive_cache {
public:
    explicit naive_cache(size_t cache_entries = 4096);

    Block* lookup(uint32_t pc);

    void install(uint64_t pc, Block&& blk);

    void invalidate_all();

    size_t capacity() const { return slots_.size(); }

private:
    size_t index_for(uint32_t start_pc) const;

    std::vector<Block> slots_;
    size_t mask_;
};

} // namespace riscv_sim

#pragma once

