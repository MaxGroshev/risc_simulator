#include "jit_instruction_factory.hpp"
#include "../hart.hpp"

namespace jit {

uint64_t memread_trampoline(Hart* hart, uint64_t addr, int size) {
    return hart->load(addr, size);
}

void memwrite_trampoline(Hart* hart, uint64_t addr, uint64_t value, int size) {
    hart->store(addr, value, size);
}

} // namespace jit
