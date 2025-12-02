#include "machine.hpp"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <elf.h>
#include <stdexcept>
#include <cstring>
#include <chrono>
#include <cassert>

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

pa_t round_up_to_page(pa_t x) {
    return (x + (PAGESIZE - 1)) & ~(PAGESIZE - 1);
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
        if (phdr.p_type != PT_LOAD)
            continue;

        // permissions
        flag_t pte_flags = 0;
        if (phdr.p_flags & PF_R) pte_flags |= PTE_R;
        if (phdr.p_flags & PF_W) pte_flags |= PTE_W;
        if (phdr.p_flags & PF_X) pte_flags |= PTE_X;
        // TODO: PTE_U user/kernel

        // compute mapping ranges
        const va_t seg_vaddr  = phdr.p_vaddr;
        const auto seg_memsz  = phdr.p_memsz;

        const va_t start_page = ((seg_vaddr & ~(PAGESIZE - 1)));
        assert(start_page != seg_vaddr);

        const va_t end_page = round_up_to_page(seg_vaddr + seg_memsz);
        const size_t page_count = (end_page - start_page) / PAGESIZE;

        file.seekg(phdr.p_offset);
        std::vector<uint8_t> seg(round_up_to_page(phdr.p_filesz), 0); // Round up to page for safe copy to physical page
        file.read(reinterpret_cast<char*>(seg.data()), phdr.p_filesz);

        for (size_t pg = 0; pg < page_count; ++pg) {
            va_t vpage = start_page + pg * PAGESIZE;
            pa_t phys = vpage; // 1:1 mapping for now

            auto seg_offset = vpage - seg_vaddr;

            if (seg_offset < seg_memsz) {
                memory_.load_data(phys, (seg.data() + seg_offset), (seg_memsz - seg_offset) % PAGESIZE);
            } else {
                memory_.zero_init(phys, PAGESIZE);
            }

            mmu_.map_page(hart_.get_context_for_MMU(), vpage, phys, pte_flags);
        }
    }

    /// TODO: it is rather ugly
    hart_.set_reg(2, StackTop);
    mmu_.map_page(hart_.get_context_for_MMU(), StackTop, StackTop, PTE_R | PTE_W);
    
    hart_.set_halt(false);
}

void Machine::run(uint64_t max_cycles) {
    uint64_t cycle = 0;
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