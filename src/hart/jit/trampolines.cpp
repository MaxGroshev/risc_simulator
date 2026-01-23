#include "jit_instruction_factory.hpp"
#include "../hart.hpp"

namespace jit {

uint64_t memread_trampoline(Hart* hart, uint64_t addr, int size) {
    return hart->load(addr, size);
}

void memwrite_trampoline(Hart* hart, uint64_t addr, uint64_t value, int size) {
    hart->store(addr, value, size);
}

uint64_t csrw_trampoline(Hart* hart, uint64_t csr, uint64_t value) {
    uint64_t old_val = hart->get_csr(static_cast<uint16_t>(csr));
    hart->set_csr(static_cast<uint16_t>(csr), value);
    return old_val;
}

void ecall_trampoline(Hart* hart) {
    hart->do_ecall();
}

void call_trampoline(Hart* hart, uint64_t target_pc, uint64_t return_pc) {
    if (!hart) {
        return;
    }
    if (hart->ensure_jit_function(target_pc)) {
        hart->execute_jitted_function(target_pc);
        return;
    }
    hart->run_until_pc(return_pc);
}

} // namespace jit
