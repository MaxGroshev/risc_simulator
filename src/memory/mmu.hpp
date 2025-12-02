#pragma once

#include <decode_execute_module/common.hpp>
#include <hart/hart_common.hpp>
#include "memory.hpp"

enum class AccessType {
    Fetch,
    Load,
    Store
};



const uint64_t PAGESIZE = 4096;
const uint64_t PTESIZE  = 8; // 64-bit PTE
const int LEVELS   = 3;      // Sv39

struct HartContext {
    reg_t satp;
    PrivilegeMode prv;
};

using va_t = reg_t;
using pa_t = reg_t;

struct TranslateResult {
    pa_t pa;
    Exception e;
};


class MMU {
public:
    explicit MMU(Memory &m) : mem_(m) {}

    TranslateResult translate(va_t va, AccessType type, const HartContext &ctx);
    
    reg_t phys_read(pa_t pa, int size) const {return mem_.read(pa, size); };
    void phys_write(pa_t pa, reg_t value, int size) { return mem_.write(pa, value, size); };
    
private:
    Memory &mem_;
};
