#ifndef MACHINE_HPP
#define MACHINE_HPP

#include "hart/hart.hpp"
#include "memory/memory.hpp"
#include <string>

class Machine {
public:
    Machine();
    Machine(const Machine&) = delete;
    Machine(Machine&&) = delete;
    Machine& operator=(const Machine&) = delete;
    Machine& operator=(Machine&&) = delete;

    ~Machine() = default;

    Hart& get_hart() { return hart_; }
    const Memory& get_memory() const { return memory_; }

    void load_elf(const std::string& filename);

    void run(uint64_t max_cycles = 0);

    void dump_regs() const;

    uint64_t memory_read(uint64_t addr, int size, bool sign_extend) const;
    void memory_write(uint64_t addr, uint64_t value, int size);

private:
    Memory memory_;
    Hart hart_;
};

#endif // MACHINE_HPP