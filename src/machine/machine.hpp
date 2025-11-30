#pragma once

#include <hart/hart.hpp>
#include <memory/mmu.hpp>
#include <memory/memory.hpp>

#include "sim_config.hpp"
#include <string>

// Memory layout constants
constexpr size_t StackSize = 0x1000;
constexpr va_t StackTop = 0x1000000ULL - StackSize;

class Machine {
public:
    Machine(sim_config_t& sim_conf) : memory_(), mmu_(memory_), hart_(mmu_, sim_conf) {
        memory_.zero_init(StackTop, StackSize);
    }
    Machine() : memory_(), mmu_(memory_), hart_(mmu_) {
        memory_.zero_init(StackTop, StackSize);
    }

    Hart& get_hart() { return hart_; }
    const Memory& get_memory() const { return memory_; }

    void load_elf(const std::string& filename);

    void run(uint64_t max_cycles = 0);

    void dump_regs() const;

private:
    Memory memory_;
    MMU mmu_;
    Hart hart_;
};
