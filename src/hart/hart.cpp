#include "hart.hpp"
// TODO(ArsenySamoylov) Consider removing hart dependency on machine
#include "machine/machine.hpp"
#include "decode_execute_module/decoder/rv32i_decoder_gen.hpp"
#include "decode_execute_module/executer/rv32i_executer_gen.hpp"
#include <iostream>
#include <stdexcept>

Hart::Hart(Memory& memory) : memory_(memory), pc_(0), next_pc_(0), halt_(false) {
    regs_.fill(0);
}

reg_t Hart::get_reg(uint8_t reg_num) const {
    if (reg_num == 0) 
        return 0U;

    if (reg_num >= 32) 
        throw std::out_of_range("Invalid register number in get_reg");

    return regs_[reg_num];
}

void Hart::set_reg(uint8_t reg_num, reg_t value) {
    if (reg_num == 0) 
        return;

    if (reg_num >= 32) 
        throw std::out_of_range("Invalid register number in set_reg");

    regs_[reg_num] = value;
}

reg_t Hart::get_pc() const {
    return pc_;
}

void Hart::set_pc(reg_t value) {
    pc_ = value;
}

void Hart::set_next_pc(reg_t value) {
    next_pc_ = value;
}

reg_t Hart::memory_read(reg_t addr, int size, bool sign_extend) const {
    return memory_.read(addr, size, sign_extend);
}

void Hart::memory_write(reg_t addr, reg_t value, int size) {
    memory_.write(addr, value, size);
}

void Hart::handle_unknown_instruction(const DecodedInstruction instr) {
    reg_t instruction = memory_read(pc_, 4, false);
    // TODO: if started using exceptions (see set/get_reg)
    //       than consider trowing exception here as well
    std::cerr << "Unknown instruction at PC: 0x" << std::hex << pc_ << std::endl;
    std::cerr << "Raw: 0x" << std::hex << instruction << std::dec << std::endl;
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
    reg_t raw_instr = memory_read(pc_, sizeof(reg_t), false);

    DecodedInstruction decoded = riscv_sim::decoder::decode(raw_instr);

    next_pc_ = pc_ + 4;

    riscv_sim::executer::execute(decoded, *this);

    pc_ = next_pc_;

    return !is_halt();
}
