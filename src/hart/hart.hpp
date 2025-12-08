#pragma once

#include <cstdint>
#include <array>

#include <memory/mmu.hpp>
#include <decode_execute_module/common.hpp>

#include "hart_common.hpp"
#include "block_cache.hpp"

// @ArsenySamoylov
//      Consider inheritance as API for ISA code generated code.
//      
//      This way we can encapsulate function like set_pc, get_pc, do_ecall, handle_unknow_instruction
//      as protected methods.
class Hart {
  public:
    Hart(MMU&, uint32_t cache_len = 1024);

    reg_t get_reg(uint8_t reg_num) const;
    void set_reg(uint8_t reg_num, reg_t value);

    void set_csr(uint16_t reg_num, reg_t value);

    reg_t get_pc() const;
    void set_pc(reg_t value);
    void set_next_pc(reg_t value);

    uint64_t step();
    void set_halt(bool value);
    bool is_halt() const;
    
    // Memory access
    reg_t load(va_t addr, int size);
    void store(va_t addr, reg_t value, int size);

    // Traps
    void do_ecall();
    void handle_exception(const Exception e);
    
    HartContext get_context_for_MMU() const;
    
private:
    uint64_t execute_cached_block(Hart& hart, riscv_sim::Block* blk);

    MMU &mmu_;
    std::array<reg_t, 32> regs_;
    reg_t pc_;
    reg_t next_pc_;
    bool halt_;

    // TODO: check that csr_satp is uint32 for rv64
    reg_t csr_satp_;
    PrivilegeMode prv_;
    
    uint32_t cache_len_;
    riscv_sim::BlockCache block_cache_;

private:
    pa_t va_to_pa (va_t va, AccessType type);
    pa_t satp_to_root_table(const reg_t satp) const;
};
