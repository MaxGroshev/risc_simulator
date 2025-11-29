#include "hart.hpp"
// TODO(ArsenySamoylov) Consider removing hart dependency on machine
#include "machine/machine.hpp"
#include "decode_execute_module/decoder/rv32i_decoder_gen.hpp"
#include "decode_execute_module/executer/rv32i_executer_gen.hpp"
#include <iostream>
#include <stdexcept>

Hart::Hart(Memory& memory, uint32_t cache_len) : memory_(memory), pc_(0), next_pc_(0), halt_(false), block_cache_(4096), cache_len_(cache_len) {
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

uint64_t Hart::step() {
    using ExecFn = riscv_sim::Block::ExecFn;

    riscv_sim::Block* blk = block_cache_.lookup(pc_);

    if (blk && blk->valid && blk->start_pc == pc_) {
        uint64_t executed = 0;
        uint64_t idx = 0;
        const size_t blk_size = blk->instrs.size();

        // std::cout << "In cached block at PC: 0x" << std::hex << pc_ << std::dec << std::endl;
        while (true) {
            if (idx >= blk_size) {
                // std::cout << "Block finished at PC: 0x" << std::hex << pc_ << std::dec << std::endl;
                pc_ = next_pc_;
                break;
            }

            ExecFn fn = nullptr;
            if (idx < blk->exec_fns.size()) 
                fn = blk->exec_fns[idx];

            const DecodedInstruction dinstr = blk->instrs[idx];

            next_pc_ = pc_ + 4;

            if (fn) {
                // std::cout << "Executing cached instruction at PC: 0x" << std::hex << pc_ << std::dec << std::endl;
                fn(dinstr, *this);
            } else {
                std::cout << "Executing uncached instruction (How it happenned?) at PC: 0x" << std::hex << pc_ << std::dec << std::endl;
                riscv_sim::executer::execute(dinstr, *this);
            }

            executed++;

            reg_t expected_next = pc_ + 4;

            pc_ = next_pc_;

            if (is_halt()) {
                break;
            }

            if (next_pc_ == expected_next) {
                // std::cout << "Falling through to next cached instruction at PC: 0x" << std::hex << next_pc_ << std::dec << std::endl;
                ++idx;
                continue;
            }

            if (next_pc_ == blk->start_pc) {
                // std::cout << "Looping back to block start at PC: 0x" << std::hex << pc_ << std::dec << std::endl;
                idx = 0;
                continue;
            }

            break;
        }

        return executed;
    }

    riscv_sim::Block new_block;
    new_block.start_pc = pc_;
    new_block.valid = false;

    uint64_t collected = 0;

    while (collected < cache_len_) {
        reg_t raw_instr = memory_read(pc_, sizeof(reg_t), false);
        DecodedInstruction dinstr = riscv_sim::decoder::decode(raw_instr);

        next_pc_ = pc_ + 4;

        ExecFn fn = riscv_sim::executer::execute(dinstr, *this);
        new_block.instrs.push_back(dinstr);
        new_block.exec_fns.push_back(fn);

        collected++;

        if (is_halt()) {
            pc_ = next_pc_;
            new_block.valid = (new_block.instrs.size() > 0);
            block_cache_.install(new_block);
            break;
        }

        // std::cout << "Executed instruction at PC: 0x" << std::hex << pc_ << ", next PC: 0x" << next_pc_ << std::dec << std::endl;

        reg_t executed_next = next_pc_;

        if (next_pc_ != pc_ + 4) {
            // std::cout << "Control flow change detected at PC: 0x" << std::hex << pc_ << ", next PC: 0x" << next_pc_ << std::dec << std::endl;
            pc_ = executed_next;
            new_block.valid = (new_block.instrs.size() > 0);
            block_cache_.install(new_block);
            break;
        }

        pc_ = next_pc_;
        // std::cout << "Falling through to next instruction at PC: 0x" << std::hex << pc_ << std::dec << std::endl;

        if (collected >= cache_len_) {
            std::cout << "Max block length reached at PC: 0x" << std::hex << pc_ << std::dec << std::endl;
            new_block.valid = (new_block.instrs.size() > 0);
            block_cache_.install(new_block);
            break;
        }
    }

    return collected;
}
