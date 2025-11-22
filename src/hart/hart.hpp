#pragma once

#include <cstdint>
#include <array>

#include <decode_execute_module/common.hpp>
#include <memory/mmu.hpp>

// @ArsenySamoylov
//      Consider inheritance as API for ISA code generated code.
//      
//      This way we can encapsulate function like set_pc, get_pc, do_ecall, handle_unknow_instruction
//      as protected methods.
class Hart {
public:
    using reg_t = uint32_t;

    explicit Hart(MMU&);

    reg_t get_reg(uint8_t) const;
    void  set_reg(uint8_t, reg_t);

    reg_t get_pc() const;
    void  set_pc(reg_t);
    void  set_next_pc(reg_t);

    bool step();
    void set_halt(bool value);
    bool is_halt() const;
    
    // Memory
    reg_t memory_read(reg_t addr, int size, bool sign_extend) const;
    void memory_write(reg_t addr, reg_t value, int size);
    
    // Traps
    void do_ecall();
    void handle_unknown_instruction(const DecodedInstruction instr);

private:
    MMU &mmu_;
    std::array<reg_t, 32> regs_;
    reg_t pc_;
    reg_t next_pc_;
    bool halt_;
};