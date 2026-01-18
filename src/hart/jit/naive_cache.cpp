#include "naive_cache.hpp"
#include <stdexcept>

namespace riscv_sim {

static inline bool is_power_of_two(size_t x) {
    return x && !(x & (x - 1));
}

naive_cache::naive_cache(size_t cache_entries) {
    if (!is_power_of_two(cache_entries))
        throw std::invalid_argument("naive_cache size must be power of two");

    slots_.resize(cache_entries);
    mask_ = cache_entries - 1;
}

Block* naive_cache::lookup(uint32_t pc) {
    size_t idx = index_for(pc);
    Block& b = slots_[idx];
    if (b.valid) {
        return &b;
    }
    return nullptr;
}

void naive_cache::install(uint64_t pc, Block&& blk) {
    if (!blk.valid) return;
    size_t idx = index_for(blk.start_pc);
    slots_[idx] = std::move(blk);
    slots_[idx].valid = true;
}

void naive_cache::invalidate_all() {
    for (auto &s : slots_) {
        s.valid = false;
        s.instrs.clear();
        s.exec_fns.clear();
    }
}

} // namespace riscv_sim
