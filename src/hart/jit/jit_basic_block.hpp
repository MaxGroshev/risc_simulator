#pragma once

#include <vector>

#include "jit_instruction_factory.hpp"
#include <asmjit/x86.h>

namespace jit {

class JITBasic_block {

typedef void (*exec)();

public:
    JITBasic_block() : rt(std::make_unique<JitRuntime>()),
                       code(std::make_unique<CodeHolder>()),
                       logger(std::make_unique<StringLogger>()) {
        code->init(rt->environment(), rt->cpu_features());
        code->set_logger(logger.get());
#if defined(__aarch64__)
        this->asma64 = std::make_unique<a64::Assembler>(code.get());

        asma64->mov(a64::x29, a64::x30);
#elif defined(__x86_64__)
        this->asmx86 = std::make_unique<x86::Assembler>(code.get());
        // Standard function prologue for x86-64 SysV with preserved callee-saved regs.
        asmx86->push(x86::rbp);
        asmx86->mov(x86::rbp, x86::rsp);
        exit_label = asmx86->new_label();
#else
        // Default to AArch64 assembler if unknown architecture at compile time
        this->asma64 = std::make_unique<a64::Assembler>(code.get());
        asma64->mov(a64::x29, a64::x30);
#endif
    };

    void add_code() {
#if defined(__aarch64__)
        asma64->ret(a64::x29);
#elif defined(__x86_64__)
        asmx86->bind(exit_label);
        // Epilogue for x86-64 SysV
        asmx86->pop(x86::rbp);
        asmx86->ret();
#else
        asma64->ret(a64::x29);
#endif
        rt->add(&executer, code.get());
        // printf("JIT exec addr: %p\n", (void*)executer);
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
    std::unique_ptr<asmjit::x86::Assembler> asmx86;
    asmjit::Label exit_label;
private:
    exec executer;
};

}
