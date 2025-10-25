#include "../include/hart.hpp"
#include "../include/machine.hpp"
#include "../decode_execute_module/decoder/rv32i_decoder_gen.hpp"
#include "../decode_execute_module/executer/rv32i_executer_gen.hpp"
#include <iostream>
#include <stdexcept>

Hart::Hart(Machine& machine) : machine_(machine), pc_(0), next_pc_(0), halt_(false) {
    regs_.fill(0);
}

uint32_t Hart::get_reg(uint8_t reg_num) const {
    if (reg_num == 0) 
        return 0U;

    if (reg_num >= 32) 
        throw std::out_of_range("Invalid register number");

    return regs_[reg_num];
}

void Hart::set_reg(uint8_t reg_num, uint32_t value) {
    if (reg_num == 0) 
        return;

    if (reg_num >= 32) 
        throw std::out_of_range("Invalid register number");

    regs_[reg_num] = value;
}

uint32_t Hart::get_pc() const {
    return pc_;
}

void Hart::set_pc(uint32_t value) {
    pc_ = value;
}

void Hart::set_next_pc(uint32_t value) {
    next_pc_ = value;
}

uint32_t Hart::memory_read(uint32_t addr, int size, bool sign_extend) const {
    return machine_.memory_read(addr, size, sign_extend);
}

void Hart::memory_write(uint32_t addr, uint32_t value, int size) {
    machine_.memory_write(addr, value, size);
}

void Hart::handle_unknown_instruction(const DecodedInstruction& instr) {
    std::cerr << "Unknown instruction opcode: " << static_cast<int>(instr.opcode) << std::endl;
    std::cerr << "Raw: 0x" << std::hex << instr.raw_instruction << std::dec << std::endl;
    std::abort();
}

void Hart::do_ecall() {
    set_halt(true); // Now simulate stop on ecall
}

void Hart::set_halt(bool value) {
    halt_ = value;
}

bool Hart::is_halt() const {
    return halt_;
}

bool Hart::step() {
    uint32_t raw_instr = memory_read(pc_, 4, false);

    DecodedInstruction decoded = riscv_sim::decoder::decode(raw_instr);

    next_pc_ = pc_ + 4;

    riscv_sim::executer::execute(decoded, *this);

    pc_ = next_pc_;

    return !is_halt();
}