#ifndef MACHINE_HPP
#define MACHINE_HPP

#include "hart.hpp"
#include "memory.hpp"
#include <string>

class Machine {
public:
    Machine();

    Hart& get_hart() { return hart_; }
    Memory& get_memory() { return memory_; }

    void load_elf(const std::string& filename);

    void run(uint64_t max_cycles = 0);

    void dump_regs() const;

    uint32_t memory_read(uint32_t addr, int size, bool sign_extend) const;
    void memory_write(uint32_t addr, uint32_t value, int size);

private:
    Memory memory_;
    Hart hart_;
};

#endif // MACHINE_HPP