#ifndef HART_HPP
#define HART_HPP

#include <cstdint>
#include <array>
#include "common.hpp"

class Machine;

class Hart {
public:
    Hart(Machine& machine);
    Hart(const Hart&) = delete;
    Hart(Hart&&) = delete;
    Hart& operator=(const Hart&) = delete;
    Hart& operator=(Hart&&) = delete;

    ~Hart() = default;

    uint32_t get_reg(uint8_t reg_num) const;
    void set_reg(uint8_t reg_num, uint32_t value);

    uint32_t get_pc() const;
    void set_pc(uint32_t value);
    void set_next_pc(uint32_t value);

    uint32_t memory_read(uint32_t addr, int size, bool sign_extend) const;
    void memory_write(uint32_t addr, uint32_t value, int size);

    void handle_unknown_instruction(const DecodedInstruction& instr);

    void do_ecall();

    void set_halt(bool value);
    bool is_halt() const;

    bool step();

private:
    Machine& machine_;
    std::array<uint32_t, 32> regs_;
    uint32_t pc_;
    uint32_t next_pc_;
    bool halt_;
};

#endif // HART_HPP