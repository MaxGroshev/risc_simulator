#include "block_cache.hpp"
#include <stdexcept>

namespace riscv_sim {

static inline bool is_power_of_two(size_t x) {
    return x && !(x & (x - 1));
}

BlockCache::BlockCache(size_t cache_entries) {
    if (!is_power_of_two(cache_entries))
        throw std::invalid_argument("BlockCache size must be power of two");

    slots_.resize(cache_entries);
    mask_ = cache_entries - 1;
}

size_t BlockCache::index_for(uint32_t start_pc) const {
    return static_cast<size_t>((start_pc / 4) & static_cast<uint32_t>(mask_));
}

Block* BlockCache::lookup(uint32_t pc) {
    size_t idx = index_for(pc);
    Block& b = slots_[idx];
    if (b.valid && b.start_pc == pc) {
        return &b;
    }
    return nullptr;
}

void BlockCache::install(const Block& blk) {
    if (!blk.valid) return;
    size_t idx = index_for(blk.start_pc);
    slots_[idx] = blk;
    slots_[idx].valid = true;
}

void BlockCache::invalidate_all() {
    for (auto &s : slots_) {
        s.valid = false;
        s.instrs.clear();
        s.exec_fns.clear();
    }
}

} // namespace riscv_sim
