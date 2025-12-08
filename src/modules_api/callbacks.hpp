#pragma once
#include <cstdint>
#include "decode_execute_module/common.hpp"
#include "hart/hart_common.hpp"
#include "memory/access.hpp"

struct PostExecInfo {
    int32_t read_reg1;
    int32_t read_reg2;
    uint64_t read_reg1_val;
    uint64_t read_reg2_val;
    int32_t dest_reg;
    uint64_t dest_reg_val;
    uint64_t imm_val;

    PostExecInfo()
      : read_reg1(-1), read_reg2(-1), read_reg1_val(0), read_reg2_val(0), dest_reg(-1), dest_reg_val(0), imm_val(0) {}
};

struct InstructionCallbackContext {
    const DecodedInstruction* instr;
    const PostExecInfo* info;
};

struct BlockHookInfo {
    uint64_t pc;
};

struct TranslateHookInfo {
    AccessType type;
    va_t va;
    pa_t pa;
    // exception - if translation failed
    Exception e;
    TranslateHookInfo() : type(AccessType::Fetch), va(0), pa(0), e(ExceptionCause::None) {}
};

struct MemAccessInfo {
    AccessType type;
    va_t va;
    pa_t pa;
    int size_bytes;
    uint64_t value;
    Exception e;
    MemAccessInfo() : type(AccessType::Load), va(0), pa(0), size_bytes(0), value(0), e(ExceptionCause::None) {}
};
