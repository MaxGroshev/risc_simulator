#include "example_module.hpp"
#include <iostream>

#include "decode_execute_module/instruction_opcodes_gen.hpp"
#include "hart/hart.hpp"

static void example_pre_cb(Hart* /*hart*/, void* payload, Module* owner) {
    auto instr = static_cast<const DecodedInstruction*>(payload);
    if (!instr) return;
    auto mod = static_cast<ExampleModule*>(owner);
    mod->stats_[instr->opcode]++;
}

static void example_post_cb(Hart* /*hart*/, void* payload, Module* owner) {
    auto ctx = static_cast<const InstructionCallbackContext*>(payload);
    if (!ctx) return;

    const PostExecInfo* info = ctx->info;
    if (!info) return;

    auto mod = static_cast<ExampleModule*>(owner);
    if (info->read_reg1 >= 0) mod->reg_freq_[info->read_reg1]++;
    if (info->read_reg2 >= 0) mod->reg_freq_[info->read_reg2]++;
    if (info->dest_reg >= 0) mod->reg_freq_[info->dest_reg]++;
}

static void example_block_start_cb(Hart* /*hart*/, void* payload, Module* owner) {
    auto bhe = static_cast<const BlockHookInfo*>(payload);
    if (!bhe) return;
    auto mod = static_cast<ExampleModule*>(owner);
    mod->block_stats_[bhe->pc]++;
}

void ExampleModule::register_callbacks(Hart& hart) {
    std::vector<InstructionOpcode> ops = {
        InstructionOpcode::ADD, InstructionOpcode::ADDW, InstructionOpcode::ADDI,
        InstructionOpcode::SUB, InstructionOpcode::SUBW
    };

    std::vector<InstructionOpcode> add_ops = {InstructionOpcode::ADD, InstructionOpcode::ADDW, InstructionOpcode::ADDI};
    hart.register_pre_execute_callback(this, add_ops, &example_pre_cb);

    std::vector<InstructionOpcode> sub_ops = {InstructionOpcode::SUB, InstructionOpcode::SUBW};
    hart.register_pre_execute_callback(this, sub_ops, &example_pre_cb);

    hart.register_post_execute_callback(this, ops, &example_post_cb);

    hart.register_block_start_callback(this, &example_block_start_cb);
}

ExampleModule::~ExampleModule() {
    std::cout << "ExampleModule statistics:\n";
    for (auto &p : stats_) {
        std::cout << "  opcode " << static_cast<int>(p.first) << ": " << p.second << "\n";
    }

    std::cout << "Register frequency:\n";
    for (auto &p : reg_freq_) {
        std::cout << "  x" << p.first << ": " << p.second << "\n";
    }

    std::cout << "Basic block execution counts:\n";
    for (auto &p : block_stats_) {
        std::cout << "  block " << p.first << ": " << p.second << "\n";
    }
}