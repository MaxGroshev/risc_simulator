#include "machine/machine.hpp"
#include <iostream>
#include <cstdint>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <elf_file> [max_cycles]" << std::endl;
        return 1;
    }

    uint64_t max_cycles = (argc > 2) ? std::stoull(argv[2]) : 0;
    Machine machine;

    try {
        machine.load_elf(argv[1]);
        machine.run(max_cycles);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}