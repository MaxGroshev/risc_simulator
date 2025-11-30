#include "hart.hpp"
// TODO(ArsenySamoylov) Consider removing hart dependency on machine
#include "machine/machine.hpp"
#include "decode_execute_module/decoder/rv32i_decoder_gen.hpp"
#include "decode_execute_module/executer/rv32i_executer_gen.hpp"
#include <iostream>
#include <stdexcept>

using ExecFn = riscv_sim::Block::ExecFn;

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

reg_t Hart::memory_read(reg_t addr, int size) const {
    return memory_.read(addr, size);
}

void Hart::memory_write(reg_t addr, reg_t value, int size) {
    memory_.write(addr, value, size);
}

void Hart::handle_unknown_instruction(const DecodedInstruction instr) {
    reg_t instruction = memory_read(pc_, 4);
    // TODO: if started using exceptions (see set/get_reg)
    //       than consider trowing exception here as well
    std::cerr << "Unknown instruction at PC: 0x" << std::hex << pc_ << std::endl;
    std::cerr << "Raw: 0x" << std::hex << instruction << std::dec << std::endl;
    std::abort();
}

void Hart::do_ecall() {
    set_halt(true);
    // reg_t syscall_num = get_reg(17); // a7
    // reg_t to_print;
    // std::cerr << "ecall invoked at PC: 0x" << std::hex << pc_ << std::dec << " syscall: " << syscall_num << std::endl;

    // switch (syscall_num) {
    //     case 1: // print integer
    //         to_print = get_reg(10); // a0
    //         std::cout << to_print << std::endl;
    //         break;
    //     case 2: // print char
    //         to_print = get_reg(10); // a0
    //         std::cout << static_cast<char>(to_print) << std::flush;
    //         break;
    //     default:
    //         std::cerr << "Unknown syscall number: " << syscall_num << std::endl;
    //         set_halt(true); // Now simulate stop on ecall
    //         break;
    // }
}

void Hart::set_halt(bool value) {
    halt_ = value;
}

bool Hart::is_halt() const {
    return halt_;
}

#ifdef DEBUG_EXECUTION
static inline void debug_cout(const std::string& msg) {
    std::cout << msg << std::endl;
}
#else
#define debug_cout(...) ((void)0)
#endif

uint64_t Hart::execute_cached_block(Hart& hart, riscv_sim::Block* blk) {
    uint64_t executed = 0;
    uint64_t idx = 0;
    const size_t blk_size = blk->instrs.size();

    debug_cout("In cached block at PC: 0x" + std::to_string(pc_));
    while (true) {
        if (idx >= blk_size) {
            debug_cout("Block finished at PC: 0x" + std::to_string(pc_));
            pc_ = next_pc_;
            break;
        }

        ExecFn fn = nullptr;
        if (idx < blk->exec_fns.size()) 
            fn = blk->exec_fns[idx];

        const DecodedInstruction dinstr = blk->instrs[idx];

        next_pc_ = pc_ + 4;

        if (fn) {
            debug_cout("Executing cached instruction at PC: 0x" + std::to_string(pc_));
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
            debug_cout("Falling through to next cached instruction at PC: 0x" + std::to_string(next_pc_));
            ++idx;
            continue;
        }

        if (next_pc_ == blk->start_pc) {
            debug_cout("Looping back to block start at PC: 0x" + std::to_string(pc_));
            idx = 0;
            continue;
        }

        break;
    }

    return executed;
}

uint64_t Hart::step() {
    riscv_sim::Block* blk = block_cache_.lookup(pc_);

    if (blk && blk->valid && blk->start_pc == pc_) {
        return execute_cached_block(*this, blk);
    }

    riscv_sim::Block new_block;
    new_block.start_pc = pc_;
    new_block.valid = false;

    uint64_t collected = 0;

    while (collected < cache_len_) {
        uint32_t raw_instr = static_cast<uint32_t>(memory_read(pc_, 4));
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

        debug_cout("Executed instruction at PC: 0x" + std::to_string(pc_) + ", next PC: 0x" + std::to_string(next_pc_));

        reg_t executed_next = next_pc_;

        if (next_pc_ != pc_ + 4) {
            debug_cout("Control flow change detected at PC: 0x" + std::to_string(pc_) + ", next PC: 0x" + std::to_string(next_pc_));
            pc_ = executed_next;
            new_block.valid = (new_block.instrs.size() > 0);
            block_cache_.install(new_block);
            break;
        }

        pc_ = next_pc_;
        debug_cout("Falling through to next instruction at PC: 0x" + std::to_string(pc_));

        if (collected >= cache_len_) {
            debug_cout("Max block length reached at PC: 0x" + std::to_string(pc_));
            new_block.valid = (new_block.instrs.size() > 0);
            block_cache_.install(new_block);
            break;
        }
    }

    return collected;
}
