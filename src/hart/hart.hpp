#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <memory>

#include <memory/mmu.hpp>
#include "threaded_code.hpp"
#include "sim_config.hpp"
#include "decode_execute_module/instruction_opcodes_gen.hpp"

#ifdef ENABLE_MODULES
#include "modules_api/module.hpp"
#endif

// @ArsenySamoylov
//      Consider inheritance as API for ISA code generated code.
//      
//      This way we can encapsulate function like set_pc, get_pc, do_ecall, handle_unknow_instruction
//      as protected methods.


class Hart {
  public:
    using reg_t = uint64_t;
    Hart(MMU&, sim_config_t& sim_conf);
    Hart(MMU&, uint32_t cache_len = 1024);

    reg_t get_reg(uint8_t reg_num) const;
    void set_reg(uint8_t reg_num, reg_t value);

    reg_t get_pc() const;
    reg_t* get_pc_ptr();
    void set_pc(reg_t value);
    void set_next_pc(reg_t value);

    uint64_t step();
    
    void set_halt(bool value);
    bool is_halt() const;

    reg_t* get_reg_file_begin();

    // Memory access
    reg_t load(va_t addr, int size);
    void store(va_t addr, reg_t value, int size);

    // Traps
    void do_ecall();
    void handle_exception(const Exception e);
    
#ifdef ENABLE_MODULES
    void add_module(std::shared_ptr<Module> mod);

    using CallbackFn = void (*)(Hart* hart, void* payload, Module* owner);

    void register_pre_execute_callback(Module* owner, const std::vector<InstructionOpcode>& ops, CallbackFn cb);
    void register_post_execute_callback(Module* owner, const std::vector<InstructionOpcode>& ops, CallbackFn cb);

    void register_block_start_callback(Module* owner, CallbackFn cb);
    void register_block_end_callback(Module* owner, CallbackFn cb);

    // Memory-related callbacks
    void register_memory_access_callback(Module* owner, CallbackFn cb);
    void register_translate_callback(Module* owner, CallbackFn cb);

    void invoke_pre_callbacks(size_t idx, const DecodedInstruction& instr);
    void invoke_post_callbacks(size_t idx, const DecodedInstruction& instr, const PostExecInfo& info);
#endif

private:

    uint64_t execute_cached_block(Hart& hart, riscv_sim::Block* blk);

    reg_t pc_;
    MMU &mmu_;
    std::array<reg_t, 32> regs_;
    reg_t next_pc_;
    bool halt_;
    
    reg_t csr_satp_;
    PrivilegeMode prv_;

    uint32_t max_cached_bb_size_;
    riscv_sim::ThreadedCode<Hart> th_code_;

#ifdef ENABLE_MODULES
    std::vector<std::shared_ptr<Module>> modules_;

    // per-opcode callbacks registered by modules
    struct CallbackEntry {
      CallbackFn fn;
      Module* owner;
    };

    std::vector<std::vector<CallbackEntry>> pre_callbacks_;
    std::vector<std::vector<CallbackEntry>> post_callbacks_;
    std::vector<CallbackEntry> block_start_callbacks_;
    std::vector<CallbackEntry> block_end_callbacks_;
    std::vector<CallbackEntry> mem_access_callbacks_;
    std::vector<CallbackEntry> translate_callbacks_;

    bool any_pre_callbacks_{false};
    bool any_post_callbacks_{false};
    bool any_block_start_callbacks_{false};
    bool any_block_end_callbacks_{false};
    bool any_mem_access_callbacks_{false};
    bool any_translate_callbacks_{false};
#endif

private:
    pa_t va_to_pa (va_t va, AccessType type);
};