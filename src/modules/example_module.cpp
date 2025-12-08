#include "example_module.hpp"
#include <iostream>

#include "decode_execute_module/instruction_opcodes_gen.hpp"
#include "hart/hart.hpp"


static void example_pre_add_cb(Hart* hart, const DecodedInstruction* instr, Module* owner) {
    auto mod = static_cast<ExampleModule*>(owner);
    mod->handle_addi(*hart, *instr);
}

static void example_pre_sub_cb(Hart* hart, const DecodedInstruction* instr, Module* owner) {
    auto mod = static_cast<ExampleModule*>(owner);
    mod->handle_sub(*hart, *instr);
}

void ExampleModule::register_callbacks(Hart& hart) {
    std::vector<InstructionOpcode> add_ops = {InstructionOpcode::ADD, InstructionOpcode::ADDW, InstructionOpcode::ADDI};
    hart.register_pre_execute_callback(this, add_ops, &example_pre_add_cb);

    // register sub-family (SUB, SUBW)
    std::vector<InstructionOpcode> sub_ops = {InstructionOpcode::SUB, InstructionOpcode::SUBW};
    hart.register_pre_execute_callback(this, sub_ops, &example_pre_sub_cb);
}

void ExampleModule::handle_addi(Hart& hart, const DecodedInstruction& instr) {
    stats_[InstructionOpcode::ADDI]++;
}

void ExampleModule::handle_sub(Hart& hart, const DecodedInstruction& instr) {
    stats_[InstructionOpcode::SUB]++;
}

ExampleModule::~ExampleModule() {
    std::cout << "ExampleModule statistics:\n";
    for (auto &p : stats_) {
        std::cout << "  opcode " << static_cast<int>(p.first) << ": " << p.second << "\n";
    }
}