#pragma once

#include <vector>

#include "jit_instruction_factory.hpp"

namespace jit {

class JITBasic_block {

typedef void (*exec)();

public:
    JITBasic_block() : rt(std::make_unique<JitRuntime>()),
                       code(std::make_unique<CodeHolder>()),
                       logger(std::make_unique<StringLogger>()) {
        code->init(rt->environment(), rt->cpu_features());
        code->set_logger(logger.get());
        this->asma64 = std::make_unique<a64::Assembler>(code.get());

        asma64->mov(a64::x29, a64::x30);
    };

    void add_code() {
        asma64->ret(a64::x29);
        rt->add(&executer, code.get());
        // dump();
    }

    void dump() {
        printf("Dump of BB code:\n %s\n", logger->content().data());
    }

    void execute() {
        executer();
    }

    std::unique_ptr<JitRuntime> rt;
    std::unique_ptr<CodeHolder> code;
    std::unique_ptr<StringLogger> logger;
    std::unique_ptr<a64::Assembler> asma64;
private:
    exec executer;
};

}