#pragma once

#include <vector>
#include "memory_types.hpp"

struct TLBEntry {
    va_t tag;       // VPN tag
    pa_t ppn;       // Physical page number
    uint32_t page_size; // 4k / 2M / 1G
    bool     valid;

    bool match(va_t va) const {
        if (!valid) return false;

        va_t va_mask = ~(page_size - 1);
        va_t va_tag  = (va & va_mask) >> 12;

        return va_tag == tag;
    }
};

inline size_t tlb_hash(va_t va, size_t size_mask) {
    va_t vpn = va >> 12;
    return vpn & size_mask;
}


class TLB {
public:
    explicit TLB(size_t size)
        : size_(size), mask_(size - 1), entries_(size) {}

    const TLBEntry* lookup(va_t va) const {
        const auto &e = entries_[tlb_hash(va, mask_)];
        return e.match(va) ? &e : nullptr;
    }

    void insert(va_t va, pa_t pa, uint32_t page_size) {
        va_t vpn = va >> 12;

        size_t idx = tlb_hash(va, mask_);
        entries_[idx] = TLBEntry{
            .tag = (va & ~(page_size - 1)) >> 12,
            .ppn = pa >> 12,
            .page_size = page_size,
            .valid = true
        };
    }

    void flush() {
        for (auto &e : entries_)
            e.valid = false;
    }

private:
    size_t size_;
    size_t mask_;
    std::vector<TLBEntry> entries_;
};
