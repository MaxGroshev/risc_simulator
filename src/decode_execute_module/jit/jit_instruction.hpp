#pragma once

#include <iostream>
#include <optional>
#include <memory>

// #include <asmjit/x86.h>
#include <asmjit/a64.h> 

namespace jit {

using jit_instr = void (*)(uint64_t*);
using namespace asmjit;

class JITInstruction {
private:
    JitRuntime* runtime;
    jit_instr function;
    
public:
    // JITInstruction(const JITInstruction&) = delete;
    // JITInstruction& operator=(const JITInstruction&) = delete;
    
    // JITInstruction(JITInstruction&& other) noexcept 
    //     : runtime(other.runtime), function(other.function) {
    //     other.function = nullptr;
    // }
    
    // JITInstruction& operator=(JITInstruction&& other) noexcept {
    //     if (this != &other) {
    //         runtime = other.runtime;
    //         function = other.function;
    //         other.function = nullptr;
    //     }
    //     return *this;
    // }
    
    JITInstruction(JitRuntime *rt, jit_instr func) 
        : runtime(rt), function(func) {}
    
    ~JITInstruction() {
        if (function && runtime) {
            runtime->release(function);
        }
    }
    
    // Call operator
    template<typename... Args>
    auto operator()(Args&&... args) -> decltype(function(std::forward<Args>(args)...)) {
        return function(std::forward<Args>(args)...);
    }
    
    // Get raw function pointer
    // jit_instr get() const { return function; }
};

}
