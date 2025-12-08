#include "machine.hpp"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <elf.h>
#include <stdexcept>
#include <cstring>
#include <chrono>
#include <memory>
#include "modules/example_module.hpp"

static void ehdr_sanity_check(const Elf64_Ehdr &ehdr) {
    if (ehdr.e_ident[0] != 0x7f || ehdr.e_ident[1] != 'E' || ehdr.e_ident[2] != 'L' || ehdr.e_ident[3] != 'F')
    {
        std::cout << ehdr.e_ident << std::endl;
        throw std::runtime_error("Not a valid ELF file");
    }

    if (ehdr.e_machine != EM_RISCV) {
        throw std::runtime_error("Not a RISC-V ELF");
    }

    if (ehdr.e_ident[EI_CLASS] != ELFCLASS64) {
        throw std::runtime_error("Only 64-bit ELF supported in RV64 mode");
    }
}

void Machine::load_elf(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) 
        throw std::runtime_error("Failed to open ELF file");

    Elf64_Ehdr ehdr;
    file.read(reinterpret_cast<char*>(&ehdr), sizeof(ehdr));
    ehdr_sanity_check(ehdr);
    
    hart_.set_pc(ehdr.e_entry);

    file.seekg(ehdr.e_phoff);
    for (int i = 0; i < ehdr.e_phnum; ++i) {
        Elf64_Phdr phdr;
        file.read(reinterpret_cast<char*>(&phdr), sizeof(phdr));
        if (phdr.p_type == PT_LOAD) {
            file.seekg(phdr.p_offset);
            std::vector<uint8_t> data(phdr.p_filesz);
            file.read(reinterpret_cast<char*>(data.data()), phdr.p_filesz);
            memory_.load_data(phdr.p_vaddr, data.data(), phdr.p_filesz);

            // for .bss
            if (phdr.p_memsz > phdr.p_filesz) {
                memory_.zero_init(phdr.p_vaddr + phdr.p_filesz, phdr.p_memsz - phdr.p_filesz);
            }
        }
    }

    /// TODO: it is rather ugly
    hart_.set_reg(2, StackBottom);
    hart_.set_halt(false);
}

void Machine::run(uint64_t max_cycles) {
    volatile uint64_t cycle = 0;
    auto start = std::chrono::high_resolution_clock::now();

    while ((max_cycles == 0 || cycle < max_cycles) && !hart_.is_halt()) {
        cycle += hart_.step();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    double time_sec = elapsed.count();

    std::cout << "Ran " << cycle << " cycles." << std::endl;
    std::cout << "Output (a0): " << hart_.get_reg(10) << std::endl; // a0 is x10

    if (cycle > 0 && time_sec > 0) {
        double mips = (static_cast<double>(cycle) / time_sec) / 1e6;
        std::cout << "Performance: " << mips << " MIPS" << std::endl;
    }
    
}

void Machine::dump_regs() const {
    for (int i = 0; i < 32; ++i) {
        std::cout << "x" << i << ": 0x" << std::hex << std::setw(16) << std::setfill('0') << hart_.get_reg(i) << std::endl;
    }
}