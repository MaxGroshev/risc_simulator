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

    std::unordered_map<InstructionOpcode, uint64_t> stats_;

    std::unordered_map<int32_t, uint64_t> reg_freq_;

    std::unordered_map<uint64_t, uint64_t> block_stats_;

    // Memory statistics
    uint64_t load_count_ = 0;
    uint64_t store_count_ = 0;
    std::unordered_map<int, uint64_t> mem_size_counts_;
};

#endif // EXAMPLE_MODULE_HPP