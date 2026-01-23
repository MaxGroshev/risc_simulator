#include "hart.hpp"

#include "decode_execute_module/decoder/rv32i_decoder_gen.hpp"
#include "decode_execute_module/executer/rv32i_executer_gen.hpp"

#ifdef ENABLE_MODULES
#include "modules_api/callbacks.hpp"
#endif

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#ifdef DEBUG_EXECUTION
static inline void debug_cout(const std::string& msg) {
    std::cout << msg << std::endl;
}
#else
#define debug_cout(...) ((void)0)
#endif
#include <functional>
#include <cassert>

using ExecFn = riscv_sim::Block::ExecFn;

Hart::Hart(MMU &mmu, sim_config_t& sim_conf) : 
    mmu_(mmu),
    next_pc_  (0), halt_(false),
    csr_satp_(0), 
    pc_       (sim_conf.initial_pc), 
    th_code_  (sim_conf, this), 
    max_cached_bb_size_(sim_conf.cached_bb_size) {

    regs_.fill(sim_conf.initial_reg_val);

#ifdef ENABLE_MODULES
    size_t opcode_count = static_cast<size_t>(InstructionOpcode::UNKNOWN) + 1;
    pre_callbacks_.resize(opcode_count);
    post_callbacks_.resize(opcode_count);
#endif
}

Hart::Hart(MMU &mmu, uint32_t cache_len) : mmu_(mmu), pc_(0), 
    next_pc_(0), halt_(false), th_code_(4096, false, this), max_cached_bb_size_(cache_len) {
    regs_.fill(0);

#ifdef ENABLE_MODULES
    size_t opcode_count = static_cast<size_t>(InstructionOpcode::UNKNOWN) + 1;
    pre_callbacks_.resize(opcode_count);
    post_callbacks_.resize(opcode_count);
#endif
}

void Hart::set_csr(uint16_t reg_num, reg_t value) {
    if (reg_num >= (1 << 12) ) 
        throw std::out_of_range("Invalid control register number in set_csr");

    if (reg_num !=  0x180)
        throw std::runtime_error("Unsupported control register number in set_csr");
    
    csr_satp_ = value;
}

Hart::reg_t Hart::get_csr(uint16_t reg_num) const {
    if (reg_num >= (1 << 12) ) 
        throw std::out_of_range("Invalid control register number in get_csr");

    if (reg_num !=  0x180)
        throw std::runtime_error("Unsupported control register number in get_csr");

    return csr_satp_;
}

Hart::reg_t Hart::get_pc() const {
    return pc_;
}

Hart::reg_t* Hart::get_pc_ptr() {
    return &(this->pc_);
}

uint64_t* Hart::get_instr_counter_ptr() {
    return &instr_counter_;
}

void Hart::set_pc(reg_t value) {
    pc_ = value;
}

void Hart::set_next_pc(reg_t value) {
    next_pc_ = value;
}

HartContext Hart::get_context_for_MMU() const {
    return HartContext{
            .root_table = satp_to_root_table(csr_satp_), 
            .mode = (csr_satp_ >> 60) & 0xF, // satp.MODE is bits 63:60
            .prv = prv_,
        };
}

uint8_t* Hart::get_memory_ptr() {
    return mmu_.get_raw_ptr();
}

size_t Hart::get_memory_size() const {
    return mmu_.get_capacity();
}

bool Hart::is_paging_disabled() const {
    return ((csr_satp_ >> 60) & 0xF) == 0;
}

template<AccessType type>
pa_t Hart::va_to_pa(va_t va) {
    auto tr = mmu_.translate<type>(va, get_context_for_MMU());

    if (!tr.e.is_none()) {
        handle_exception(tr);
    }

#ifdef ENABLE_MODULES
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
#endif

    return tr.pa;
}

pa_t Hart::satp_to_root_table(const reg_t satp) const {
    // satp.PPN lower 44 bits - physical page-number of root table
    constexpr pa_t SATP_PPN_MASK = ((1ull << 44) - 1);
    return (satp & SATP_PPN_MASK) * PAGESIZE; 
}

reg_t Hart::load(reg_t va, int size) {
    pa_t pa = va_to_pa<AccessType::Load>(va);
    reg_t val = mmu_.mem_load(pa, size);

#ifdef ENABLE_MODULES
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
#endif

    return val;
}

uint32_t Hart::fetch(reg_t va) {
    // TODO: add hook
    return mmu_.mem_load(va_to_pa<AccessType::Fetch>(va), 4);
}

void Hart::store(reg_t va, reg_t value, int size) {
    // std::cerr << "Hart::store called: va=0x" << std::hex << va << " value=0x" << value << " size=" << std::dec << size << std::endl;
    pa_t pa = va_to_pa<AccessType::Store>(va);
    mmu_.mem_store(pa, value, size);

#ifdef ENABLE_MODULES
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
#endif

    return;
}

void Hart::handle_exception(const Exception e) {
    std::cerr << "Exception: " << e.to_string() << std::endl;
    std::cerr << "PC:" <<  std::hex << pc_ << std::dec << std::endl;
    std::abort();
}

void Hart::handle_exception(const TranslateResult tr) {
    std::cerr << "MMU Error: " << tr.to_string() << std::endl;
    std::cerr << "PC:" <<  std::hex << pc_ << std::dec << std::endl;
    std::abort();
}

// NOTE(mgroshev): Need this to write in reg file from asm(jit)
Hart::reg_t* Hart::get_reg_file_begin() {
    return regs_.data();
}

void Hart::set_exec_ranges(std::vector<CodeRange> ranges) {
    exec_ranges_ = std::move(ranges);
}

bool Hart::predecode_and_jit_if_small() {
    if (!th_code_.is_jit_enabled() || exec_ranges_.empty()) {
        return false;
    }

    std::vector<uint64_t> worklist;
    std::unordered_set<uint64_t> seen_entries;
    std::vector<riscv_sim::Block> blocks;
    size_t total_instrs = 0;

    worklist.push_back(pc_);

    while (!worklist.empty()) {
        uint64_t entry_pc = worklist.back();
        worklist.pop_back();

        if (!is_exec_pc(entry_pc) || seen_entries.count(entry_pc) != 0) {
            continue;
        }
        seen_entries.insert(entry_pc);

        riscv_sim::Block blk;
        std::vector<uint64_t> call_targets;
        if (!build_function_block(entry_pc, blk, call_targets)) {
            continue;
        }

        total_instrs += blk.instrs.size();
        blocks.push_back(std::move(blk));

        for (auto target : call_targets) {
            if (seen_entries.count(target) == 0) {
                worklist.push_back(target);
            }
        }
    }

    const size_t capacity = th_code_.cache_capacity();
    if (capacity == 0 || blocks.size() > capacity) {
        return false;
    }

    const size_t max_instrs = capacity * max_cached_bb_size_;
    if (total_instrs > max_instrs) {
        return false;
    }

    for (auto& blk : blocks) {
        th_code_.install_and_jit(std::move(blk));
    }

    return true;
}

bool Hart::ensure_jit_function(uint64_t entry_pc) {
    if (!th_code_.is_jit_enabled()) {
        return false;
    }

    if (auto* blk = th_code_.lookup(entry_pc); blk && blk->get_is_jitted()) {
        return true;
    }

    riscv_sim::Block blk;
    std::vector<uint64_t> call_targets;
    if (!build_function_block(entry_pc, blk, call_targets)) {
        return false;
    }
    return th_code_.install_and_jit(std::move(blk));
}

void Hart::execute_jitted_function(uint64_t entry_pc) {
    if (auto* blk = th_code_.lookup(entry_pc); blk && blk->get_is_jitted()) {
        blk->jitted_bb->execute();
    }
}

void Hart::run_until_pc(uint64_t target_pc) {
    while (!halt_ && pc_ != target_pc) {
        step();
    }
}

bool Hart::build_function_block(uint64_t entry_pc, riscv_sim::Block& blk, std::vector<uint64_t>& call_targets) {
    if (!is_exec_pc(entry_pc)) {
        return false;
    }

    std::unordered_map<uint64_t, DecodedInstruction> instrs_by_pc;
    std::unordered_set<uint64_t> visited;
    std::vector<uint64_t> worklist;

    worklist.push_back(entry_pc);

    while (!worklist.empty()) {
        uint64_t pc = worklist.back();
        worklist.pop_back();

        if (!is_exec_pc(pc) || visited.count(pc) != 0) {
            continue;
        }
        visited.insert(pc);

        uint32_t raw_instr = static_cast<uint32_t>(fetch(pc));
        DecodedInstruction dinstr = riscv_sim::decoder::decode(raw_instr);
        instrs_by_pc.emplace(pc, dinstr);

        if (dinstr.opcode == InstructionOpcode::ECALL) {
            continue;
        }

        if (dinstr.opcode == InstructionOpcode::JAL) {
            uint64_t target = pc + static_cast<int64_t>(dinstr.imm);
            if (dinstr.rd != 0) {
                if (is_exec_pc(target)) {
                    call_targets.push_back(target);
                }
                worklist.push_back(pc + 4);
            } else {
                if (is_exec_pc(target)) {
                    worklist.push_back(target);
                }
            }
            continue;
        }

        if (dinstr.opcode == InstructionOpcode::JALR) {
            bool is_ret = (dinstr.rd == 0 && dinstr.rs1 == 1 && dinstr.imm == 0);
            if (is_ret) {
                continue;
            }
            if (dinstr.rd != 0) {
                worklist.push_back(pc + 4);
            }
            continue;
        }

        if (dinstr.format == InstructionFormat::B) {
            uint64_t target = pc + static_cast<int64_t>(dinstr.imm);
            if (is_exec_pc(target)) {
                worklist.push_back(target);
            }
            worklist.push_back(pc + 4);
            continue;
        }

        worklist.push_back(pc + 4);
    }

    if (instrs_by_pc.empty()) {
        return false;
    }

    std::vector<uint64_t> pcs;
    pcs.reserve(instrs_by_pc.size());
    for (const auto& kv : instrs_by_pc) {
        pcs.push_back(kv.first);
    }
    std::sort(pcs.begin(), pcs.end());

    auto start_it = std::find(pcs.begin(), pcs.end(), entry_pc);
    if (start_it == pcs.end()) {
        return false;
    }

    std::vector<uint64_t> ordered;
    ordered.reserve(pcs.size());
    ordered.push_back(entry_pc);
    for (auto it = std::next(start_it); it != pcs.end(); ++it) {
        ordered.push_back(*it);
    }
    for (auto it = pcs.begin(); it != start_it; ++it) {
        ordered.push_back(*it);
    }

    blk.start_pc = static_cast<uint32_t>(entry_pc);
    blk.valid = true;
    blk.is_function_block = true;
    blk.instrs.clear();
    blk.exec_fns.clear();
    blk.instr_pcs.clear();
    blk.instrs.reserve(ordered.size());
    blk.instr_pcs.reserve(ordered.size());

    for (auto pc : ordered) {
        blk.instr_pcs.push_back(pc);
        blk.instrs.push_back(instrs_by_pc[pc]);
    }

    return true;
}

bool Hart::is_exec_pc(uint64_t pc) const {
    for (const auto& range : exec_ranges_) {
        if (pc >= range.start && pc < range.end) {
            return true;
        }
    }
    return false;
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

uint64_t Hart::execute_cached_block(Hart& hart, riscv_sim::Block* blk) {
    if(blk->get_is_jitted()) {  
        // blk->jitted_bb.dump();
        if (blk->is_function_block) {
            uint64_t before = instr_counter_;
            blk->jitted_bb->execute();
            return instr_counter_ - before;
        }
        blk->jitted_bb->execute();
        return blk->instrs.size();
    }
    uint64_t executed = 0;
    uint64_t idx = 0;
    const size_t blk_size = blk->instrs.size();

    auto* fn_ptr = blk->exec_fns.data();
    auto* instrs_ptr = blk->instrs.data();

    debug_cout("In cached block at PC: 0x" + std::to_string(pc_));

#ifdef ENABLE_MODULES
    if (any_block_start_callbacks_) {
        if (!block_start_callbacks_.empty()) {
            BlockHookInfo bhe{ static_cast<uint64_t>(blk->start_pc) };
            for (const auto &entry : block_start_callbacks_) {
                entry.fn(this, (void*)&bhe, entry.owner);
            }
        }
    }
#endif
    
    while (true) {
        if (idx >= blk_size) {
            debug_cout("Block finished at PC: 0x" + std::to_string(pc_));
            
#ifdef ENABLE_MODULES
            if (any_block_end_callbacks_) {
                if (!block_end_callbacks_.empty()) {
                    BlockHookInfo bhe{ static_cast<uint64_t>(blk->start_pc) };
                    for (const auto &entry : block_end_callbacks_) {
                        entry.fn(this, (void*)&bhe, entry.owner);
                    }
                }
            }
#endif

            pc_ = next_pc_;
            break;
        }

        ExecFn fn = fn_ptr[idx];
        const DecodedInstruction& dinstr = instrs_ptr[idx];

        next_pc_ = pc_ + 4;

        fn(dinstr, *this);

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
    instr_counter_ += executed;
    return executed;
}

uint64_t Hart::step() {
    riscv_sim::Block* blk = th_code_.lookup(pc_);

    if (blk) {
        return execute_cached_block(*this, blk);
    }

    if (th_code_.is_jit_enabled()) {
        riscv_sim::Block jit_block;
        std::vector<uint64_t> call_targets;
        if (build_function_block(pc_, jit_block, call_targets)) {
            if (th_code_.install_and_jit(std::move(jit_block))) {
                if (auto* jitted_blk = th_code_.lookup(pc_)) {
                    return execute_cached_block(*this, jitted_blk);
                }
            }
        }
    }

    riscv_sim::Block new_block;
    new_block.start_pc = pc_;
    new_block.valid = false;

    uint64_t collected = 0;

    while (collected < max_cached_bb_size_) {
        uint32_t raw_instr = static_cast<uint32_t>(fetch(pc_));
        DecodedInstruction dinstr = riscv_sim::decoder::decode(raw_instr);

        debug_cout("Executing instruction with opcode " + std::to_string(static_cast<size_t>(dinstr.opcode)) + " at PC: 0x" + std::to_string(pc_));

        next_pc_ = pc_ + 4;

        ExecFn fn = riscv_sim::executer::execute(dinstr, *this);

        collected++;

        if (is_halt()) {
            pc_ = next_pc_;
            th_code_.install_bb_if_valid(std::move(new_block));
            break;
        }

        debug_cout("Executed instruction at PC: 0x" + std::to_string(pc_) + ", next PC: 0x" + std::to_string(next_pc_));

        reg_t executed_next = next_pc_;

        if ((next_pc_ != pc_ + 4)) {
            debug_cout("Control flow change detected at PC: 0x" + std::to_string(pc_) + ", next PC: 0x" + std::to_string(next_pc_));
            pc_ = executed_next;
            th_code_.install_bb_if_valid(std::move(new_block));
            break;
        }
        
        new_block.instrs.push_back(dinstr);
        new_block.exec_fns.push_back(fn);
        pc_ = next_pc_;
        debug_cout("Falling through to next instruction at PC: 0x" + std::to_string(pc_));

        if (collected >= max_cached_bb_size_) {
            debug_cout("Max block length reached at PC: 0x" + std::to_string(pc_));
            th_code_.install_bb_if_valid(std::move(new_block));
            break;
        }
    }

    instr_counter_ += collected;
    return collected;
}

#ifdef ENABLE_MODULES
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
#endif
