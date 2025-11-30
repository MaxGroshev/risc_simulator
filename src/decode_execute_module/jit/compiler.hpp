#pragma once

#include <cassert>
#include <iostream>
#include <memory>
#include <utility>

#include <asmjit/x86.h>
#include <asmjit/a64.h> 

#include "jit_instruction_factory.hpp"
#include "jit_basic_block.hpp"

#include "utils/cache.hpp"

class Hart;

namespace jit {

class JITImpl {
    
public:
    JITImpl() : rt(std::make_unique<JitRuntime>()) {};

    bool is_compiled(uint64_t label) {
        // if(bb_cache.Get(label).has_value() && bb_cache.Get(label).value().first > jit_bound) {
        //     return true;
        // }
        return false;
    }
    
    void run_compiled(uint64_t label) {
        bb_cache.Get(label).value().second.run();
    }

    void increase_access_rate_and_compile_if_ready(uint64_t label, Hart& hart) {
        // auto c_elem = bb_cache.Get(label);
        // if (!c_elem.has_value()) {
        //     auto elem = std::pair{1, JITBasic_block{}};
        //     bb_cache.Put(label, elem);
        // } else {
        //     c_elem.value().first++;
        //     if(c_elem.value().first == jit_bound) {
        //         auto bb = compile_bb(label, hart);
        //         bb_cache.Put(label, {c_elem.value().first, std::move(bb)});
        //     } else if (c_elem.value().first < jit_bound) {
        //         bb_cache.Put(label, {c_elem.value().first, std::move(c_elem.value().second)});
        //     } else {
        //         assert(false && __PRETTY_FUNCTION__);
        //     }
        // }
    }
    
    
private:
    JITBasic_block compile_bb(uint64_t label, Hart& hart) {  
        // while(true) {
            // auto instr = hart.fetch_instr(label);

        // }
        // auto a = new JitRuntime{};
    }

    void compile_inst(uint64_t label) {

    }
    const uint64_t jit_bound = 10; 
    // TODO(mgroshev): make typedef for label type  
    // label -> {NumOfUsages, CompiledBB \/ nullptr}(probably std::optional)
    utils::lru_cache<uint64_t, std::pair<uint64_t, JITBasic_block>> bb_cache;
    std::unique_ptr<JitRuntime> rt;
    // std::unordered_map<uint64_t, JITBasic_block> bb_storage;
};

}