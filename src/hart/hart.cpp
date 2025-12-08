#include "hart.hpp"

#include "decode_execute_module/decoder/rv32i_decoder_gen.hpp"
#include "decode_execute_module/executer/rv32i_executer_gen.hpp"
#include "modules_api/callbacks.hpp"

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cassert>

using ExecFn = riscv_sim::Block::ExecFn;

Hart::Hart(MMU &mmu, uint32_t cache_len) : mmu_(mmu), pc_(0), next_pc_(0), halt_(false), block_cache_(4096), cache_len_(cache_len) {
    regs_.fill(0);

    size_t opcode_count = static_cast<size_t>(InstructionOpcode::UNKNOWN) + 1;
    pre_callbacks_.resize(opcode_count);
    post_callbacks_.resize(opcode_count);
}

reg_t Hart::get_reg(uint8_t reg_num) const {
    if (reg_num == 0) 
        return 0;

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

pa_t Hart::va_to_pa(va_t va, AccessType type) {
    auto tr = mmu_.translate(va, type, HartContext{.satp = csr_satp_, .prv = prv_});

    if (!tr.e.is_none()) {
        handle_exception(tr.e);
    }

    if (any_translate_callbacks_) {
        if (!translate_callbacks_.empty()) {
            TranslateHookInfo thi;
            thi.type = type;
            thi.va = va;
            thi.pa = tr.pa;
            thi.e = tr.e;
            for (const auto &entry : translate_callbacks_) {
                entry.fn(this, (void*)&thi, entry.owner);
            }
        }
    }

    return tr.pa;
}

reg_t Hart::load(reg_t va, int size) {
    pa_t pa = va_to_pa(va, AccessType::Load);
    reg_t val = mmu_.phys_read(pa, size);

    if (any_mem_access_callbacks_) {
        if (!mem_access_callbacks_.empty()) {
            MemAccessInfo mai;
            mai.type = AccessType::Load;
            mai.va = va;
            mai.pa = pa;
            mai.size_bytes = size;
            mai.value = val;
            mai.e = Exception(ExceptionCause::None);
            for (const auto &entry : mem_access_callbacks_) {
                entry.fn(this, (void*)&mai, entry.owner);
            }
        }
    }

    return val;
}

void Hart::store(reg_t va, reg_t value, int size) {
    pa_t pa = va_to_pa(va, AccessType::Store);
    mmu_.phys_write(pa, value, size);

    if (any_mem_access_callbacks_) {
        if (!mem_access_callbacks_.empty()) {
            MemAccessInfo mai;
            mai.type = AccessType::Store;
            mai.va = va;
            mai.pa = pa;
            mai.size_bytes = size;
            mai.value = value;
            mai.e = Exception(ExceptionCause::None);
            for (const auto &entry : mem_access_callbacks_) {
                entry.fn(this, (void*)&mai, entry.owner);
            }
        }
    }

    return;
}

void Hart::handle_exception(const Exception e) {
    std::cerr << "Exception: " << e.to_string() << std::endl;
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

    if (any_block_start_callbacks_) {
        if (!block_start_callbacks_.empty()) {
            BlockHookInfo bhe{ static_cast<uint64_t>(blk->start_pc) };
            for (const auto &entry : block_start_callbacks_) {
                entry.fn(this, (void*)&bhe, entry.owner);
            }
        }
    }
    
    while (true) {
        if (idx >= blk_size) {
            debug_cout("Block finished at PC: 0x" + std::to_string(pc_));
            
            if (any_block_end_callbacks_) {
                if (!block_end_callbacks_.empty()) {
                    BlockHookInfo bhe{ static_cast<uint64_t>(blk->start_pc) };
                    for (const auto &entry : block_end_callbacks_) {
                        entry.fn(this, (void*)&bhe, entry.owner);
                    }
                }
            }

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
        uint32_t raw_instr = static_cast<uint32_t>(load(pc_, 4));
        DecodedInstruction dinstr = riscv_sim::decoder::decode(raw_instr);

        debug_cout("Executing instruction with opcode " + std::to_string(static_cast<size_t>(dinstr.opcode)) + " at PC: 0x" + std::to_string(pc_));

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

void Hart::add_module(std::shared_ptr<Module> mod) {
    modules_.push_back(mod);
}

void Hart::register_pre_execute_callback(Module* owner, const std::vector<InstructionOpcode>& ops, CallbackFn cb) {
    if (!cb) 
        return;

    for (auto op : ops) {
        size_t idx = static_cast<size_t>(op);
        if (idx >= pre_callbacks_.size()) 
            continue;

        pre_callbacks_[idx].push_back(CallbackEntry{cb, owner});

        // Ensure generated executor has a dispatcher installed for this opcode index.
        riscv_sim::executer::ensure_pre_dispatcher_installed(idx);
    }
    any_pre_callbacks_ = true;
}

void Hart::register_post_execute_callback(Module* owner, const std::vector<InstructionOpcode>& ops, CallbackFn cb) {
    if (!cb) 
        return;

    for (auto op : ops) {
        size_t idx = static_cast<size_t>(op);
        if (idx >= post_callbacks_.size()) 
            continue;

        post_callbacks_[idx].push_back(CallbackEntry{cb, owner});

        // Ensure generated executor has a dispatcher installed for this opcode index.
        riscv_sim::executer::ensure_post_dispatcher_installed(idx);
    }

    any_post_callbacks_ = true;
}

void Hart::register_block_start_callback(Module* owner, CallbackFn cb) {
    if (!cb) 
        return;
    block_start_callbacks_.push_back(CallbackEntry{cb, owner});
    
    any_block_start_callbacks_ = true;
}

void Hart::register_block_end_callback(Module* owner, CallbackFn cb) {
    if (!cb) 
        return;
    block_end_callbacks_.push_back(CallbackEntry{cb, owner});

    any_block_end_callbacks_ = true;
}

void Hart::register_memory_access_callback(Module* owner, CallbackFn cb) {
    if (!cb)
        return;
    mem_access_callbacks_.push_back(CallbackEntry{cb, owner});

    any_mem_access_callbacks_ = true;
}

void Hart::register_translate_callback(Module* owner, CallbackFn cb) {
    if (!cb)
        return;
    translate_callbacks_.push_back(CallbackEntry{cb, owner});

    any_translate_callbacks_ = true;
}

void Hart::invoke_pre_callbacks(size_t idx, const DecodedInstruction& instr) {
    if (idx >= pre_callbacks_.size())
        return;
    auto &vec = pre_callbacks_[idx];
    if (vec.empty()) 
        return;

    for (const auto &entry : vec) {
        entry.fn(this, (void*)&instr, entry.owner);
    }
}

void Hart::invoke_post_callbacks(size_t idx, const DecodedInstruction& instr, const PostExecInfo& info) {
    if (idx >= post_callbacks_.size()) 
        return;

    auto &vec = post_callbacks_[idx];
    if (vec.empty()) 
        return;

    InstructionCallbackContext ctx{ &instr, &info };
    for (const auto &entry : vec) {
        entry.fn(this, (void*)&ctx, entry.owner);
    }
}
