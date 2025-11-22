#pragma once

#include <hart/hart.hpp>
#include <memory/mmu.hpp>
#include <memory/memory.hpp>

#include <string>

class Machine {
public:
    void load_elf(const std::string& filename);

    void run(uint64_t max_cycles = 0);

    void dump_regs() const;

    // @ArsenySamoylov: Maybe memory_read/write should work with 64 bytes? (or something like this, that is close to the hardware)
    uint32_t memory_read(uint32_t addr, int size, bool sign_extend) const;
    void     memory_write(uint32_t addr, uint32_t value, int size);

private:
    Memory memory_;
    MMU mmu_;
    Hart hart_;
};
