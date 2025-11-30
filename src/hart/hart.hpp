#ifndef HART_HPP
#define HART_HPP

#include <cstdint>
#include <array>

#include "memory/memory.hpp"
#include "decode_execute_module/common.hpp"
#include "block_cache.hpp"

using reg_t = uint64_t;

class Hart {
  public:
    Hart(Memory&, uint32_t cache_len = 1024);
    Hart(const Hart&) = delete;
    Hart(Hart&&) = delete;
    Hart& operator=(const Hart&) = delete;
    Hart& operator=(Hart&&) = delete;

    ~Hart() = default;

    reg_t get_reg(uint8_t reg_num) const;
    void set_reg(uint8_t reg_num, reg_t value);

    reg_t get_pc() const;
    void set_pc(reg_t value);
    void set_next_pc(reg_t value);

    
    void handle_unknown_instruction(const DecodedInstruction instr);
    
    void do_ecall();
    
    void set_halt(bool value);
    bool is_halt() const;
    
    uint64_t step();

    reg_t memory_read(reg_t addr, int size) const;
    void memory_write(reg_t addr, reg_t value, int size);

  private:
    uint64_t execute_cached_block(Hart& hart, riscv_sim::Block* blk);

    Memory& memory_;
    std::array<reg_t, 32> regs_;
    reg_t pc_;
    reg_t next_pc_;
    bool halt_;
    
    uint32_t cache_len_;
    riscv_sim::BlockCache block_cache_;

};

#endif // HART_HPP