#ifndef MACHINE_HPP
#define MACHINE_HPP

#include "hart.hpp"
#include "memory.hpp"
#include <string>

class Machine {
public:
    Machine();

    // Accessors
    Hart& get_hart() { return hart_; }
    Memory& get_memory() { return memory_; }

    // ELF loading
    void load_elf(const std::string& filename);

    // Run simulation for max_cycles or until halt
    void run(uint64_t max_cycles = 0);

    // Dump registers for debugging
    void dump_regs() const;

    // Delegated memory access (for Hart)
    uint32_t memory_read(uint32_t addr, int size, bool sign_extend) const;
    void memory_write(uint32_t addr, uint32_t value, int size);

private:
    Memory memory_;
    Hart hart_;
};

#endif // MACHINE_HPP