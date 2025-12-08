#ifndef EXAMPLE_MODULE_HPP
#define EXAMPLE_MODULE_HPP

#include "modules_api/module.hpp"
#include <unordered_map>
#include <memory>

class Hart;
#include "decode_execute_module/instruction_opcodes_gen.hpp"

class ExampleModule : public Module {
public:
    // Register this module's callbacks with a hart.
    void register_callbacks(Hart& hart) override;

    ~ExampleModule();

    void handle_addi(Hart& hart, const DecodedInstruction& instr);
    void handle_sub(Hart& hart, const DecodedInstruction& instr);

    std::unordered_map<InstructionOpcode, uint64_t> stats_;
};

#endif // EXAMPLE_MODULE_HPP