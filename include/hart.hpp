#ifndef HART_HPP
#define HART_HPP

#include <cstdint>
#include <array>

#include "common.hpp"  // For DecodedInstruction    

class Machine;  // Forward declaration

class Hart {
public:
    Hart(Machine* machine);

    uint32_t get_reg(uint8_t reg_num) const;
    void set_reg(uint8_t reg_num, uint32_t value);

    uint32_t get_pc() const;
    void set_pc(uint32_t value);
    void set_next_pc(uint32_t value);

    // Memory access API (delegates to Machine's memory)
    uint32_t memory_read(uint32_t addr, int size, bool sign_extend) const;
    void memory_write(uint32_t addr, uint32_t value, int size);

    // Handle unknown instruction (for now, just print or throw)
    void handle_unknown_instruction(const DecodedInstruction& instr);

    void do_ecall();

    // Halt control
    void set_halt(bool value);
    bool is_halt() const;

    // Execute a single cycle: fetch, decode, execute, update PC
    bool step();

private:
    Machine* machine_;  // Reference to owning Machine for memory/IO
    std::array<uint32_t, 32> regs_;  // Register file, x0 always 0
    uint32_t pc_;  // Program counter
    uint32_t next_pc_;  // Next PC (for branches/jumps)
    bool halt_; // Halt flag
};

#endif // HART_HPP