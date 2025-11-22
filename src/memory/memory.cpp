#include "memory.hpp"
#include <stdexcept>
#include <cstring>
#include <iostream>

Memory::Memory(size_t size) : mem_(size, 0) {}

uint32_t Memory::read(uint32_t addr, int size) const {
    if (addr + size > mem_.size())
        throw std::out_of_range("Memory read out of bounds");
    return mem_[addr];
}

void Memory::write(uint32_t addr, uint32_t value, int size) {
    if (addr + size > mem_.size()) {
        std::cout << "Attempted write of size " << size << " at address 0x" << std::hex << addr << std::dec << std::endl;
        throw std::out_of_range("Memory write out of bounds");
    }

    std::memcpy(&mem_[addr], &value, size);
}

void Memory::load_data(uint32_t addr, const uint8_t* data, size_t size) {
    if (addr + size > mem_.size()) 
        throw std::out_of_range("Memory load out of bounds");

    std::memcpy(&mem_[addr], data, size);
}

void Memory::zero_init(uint32_t addr, size_t size) {
    if (addr + size > mem_.size()) 
        throw std::out_of_range("Memory zero-init out of bounds");

    std::memset(&mem_[addr], 0, size);
}

size_t Memory::size() const {
    return mem_.size();
}