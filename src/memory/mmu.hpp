#pragma once
#include <sstream>
#include <hart/hart_common.hpp>
#include "memory.hpp"
#include "tlb.hpp"


// MUST BE IN SYNC WITH memory.cpp !!!
constexpr uint64_t PAGESIZE = 4096;

constexpr uint64_t PTESIZE  = 8; // 64-bit PTE
constexpr int LEVELS   = 3;      // Sv39

constexpr flag_t PTE_V = 1 << 0;
constexpr flag_t PTE_R = 1 << 1;
constexpr flag_t PTE_W = 1 << 2;
constexpr flag_t PTE_X = 1 << 3;

// TODO to be added
// constexpr uint64_t PTE_U = 1 << 4;
// constexpr uint64_t PTE_G = 1 << 5;
// constexpr uint64_t PTE_A = 1 << 6;
// constexpr uint64_t PTE_D = 1 << 7;

constexpr uint64_t PTE_PPN_SHIFT = 10;

struct HartContext {
    pa_t root_table;
    reg_t mode;
    PrivilegeMode prv;
};

constexpr int TLB_SIZE = 64;

struct TranslateResult {
    pa_t pa;
    Exception e;
    va_t faulting_addr;
    AccessType access;

    std::string to_string() const {
        std::ostringstream oss;
        oss << e.to_string();
        oss << "\nFault type: ";
            switch(access) {
                case AccessType::Fetch: 
                    oss << "Fetch";
                    break;
                case AccessType::Load:  
                    oss << "Load";
                    break;
                case AccessType::Store: 
                    oss << "Store";
                    break;
                default:
                    oss << "Unknow";
            }
        
        oss << "\nFaulting Address: " << std::hex << faulting_addr << std::dec;
        return oss.str();
    } 
};


class MMU {
public:
    explicit MMU(Memory &m) : 
        mem_(m),
        itlb_  (TLB_SIZE),
        dtlb_r_(TLB_SIZE),
        dtlb_w_(TLB_SIZE) 
        {}

    uint8_t* get_raw_ptr() { return mem_.data(); }
    size_t get_capacity() const { return mem_.size(); }

    template<AccessType type>
    TranslateResult translate(va_t va, const HartContext ctx) {
        
        if (ctx.mode == 0) {
            return { .pa = va, .e = Exception{ExceptionCause::None} };
        }
        
        const TLBEntry* hit = nullptr;
        if constexpr (type == AccessType::Fetch)
            hit = itlb_.lookup(va);
        else if constexpr (type == AccessType::Load)
            hit = dtlb_r_.lookup(va);
        else if constexpr (type == AccessType::Store)
            hit = dtlb_w_.lookup(va);

        if (hit) {
            pa_t pa = (hit->ppn << 12) | (va & (hit->page_size - 1));
            return {.pa = pa, .e = Exception{ExceptionCause::None}};
        }

        return translate_generic(va, type, ctx);
    }
    
    reg_t mem_load(pa_t pa, int size) const {return mem_.read(pa, size); };
    void mem_store(pa_t pa, reg_t value, int size) { return mem_.write(pa, value, size); };

private:
    Memory &mem_;
    TranslateResult translate_generic(va_t va, AccessType type, const HartContext ctx);

    TLB itlb_;
    TLB dtlb_r_;
    TLB dtlb_w_;
};
