#include "../include/machine.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <elf.h>  // For ELF structures
#include <stdexcept>
#include <cstring>

Machine::Machine() : hart_(this) {}  // Hart references this Machine

uint32_t Machine::memory_read(uint32_t addr, int size, bool sign_extend) const {
    return memory_.read(addr, size, sign_extend);
}

void Machine::memory_write(uint32_t addr, uint32_t value, int size) {
    memory_.write(addr, value, size);
}

void Machine::load_elf(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) throw std::runtime_error("Failed to open ELF file");

    Elf32_Ehdr ehdr;
    file.read(reinterpret_cast<char*>(&ehdr), sizeof(ehdr));
    if (ehdr.e_ident[0] != 0x7f || ehdr.e_ident[1] != 'E' || ehdr.e_ident[2] != 'L' || ehdr.e_ident[3] != 'F')
    {
        std::cout << ehdr.e_ident << std::endl;
        throw std::runtime_error("Not a valid ELF file");
    }
    if (ehdr.e_machine != EM_RISCV) {
        throw std::runtime_error("Not a RISC-V ELF");
    }

    // Set entry point
    hart_.set_pc(ehdr.e_entry);
    memory_.zero_init(0, 16 * 1024 * 1024);  // Clear memory

    // Read program headers
    file.seekg(ehdr.e_phoff);
    for (int i = 0; i < ehdr.e_phnum; ++i) {
        Elf32_Phdr phdr;
        file.read(reinterpret_cast<char*>(&phdr), sizeof(phdr));
        if (phdr.p_type == PT_LOAD) {
            // Load segment
            file.seekg(phdr.p_offset);
            std::vector<uint8_t> data(phdr.p_filesz);
            file.read(reinterpret_cast<char*>(data.data()), phdr.p_filesz);
            memory_.load_data(phdr.p_vaddr, data.data(), phdr.p_filesz);

            // Zero-init remaining (for .bss)
            if (phdr.p_memsz > phdr.p_filesz) {
                memory_.zero_init(phdr.p_vaddr + phdr.p_filesz, phdr.p_memsz - phdr.p_filesz);
            }
        }
    }

    // Set stack pointer (high address, e.g., end of memory - some offset)
    hart_.set_reg(2, static_cast<uint32_t>(16 * 1024 * 1024 - 0x1000));  // sp = mem_size - 4KB

    hart_.set_halt(false);
}

void Machine::run(uint64_t max_cycles) {
    uint64_t cycle = 0;
    while ((max_cycles == 0 || cycle < max_cycles) && !hart_.is_halt()) {
        if (!hart_.step()) break;  // If step returns false, halt
        ++cycle;
    }
    std::cout << "Ran " << cycle << " cycles." << std::endl;
    std::cout << "Output (a0): " << hart_.get_reg(10) << std::endl; // a0 is x10
}

void Machine::dump_regs() const {
    for (int i = 0; i < 32; ++i) {
        std::cout << "x" << i << ": 0x" << std::hex << std::setw(8) << std::setfill('0') << hart_.get_reg(i) << std::endl;
    }
}