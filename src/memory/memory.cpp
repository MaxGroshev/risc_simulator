#include "memory.hpp"

#include <stdexcept>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>

/// TODO: exceptions should be processed properly, now they are not caught

Memory::Memory(size_t size) : backing_(nullptr), capacity_(0) {
    if (size == 0) {
        throw std::invalid_argument("Memory size must be > 0");
    }

    const long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) 
        throw std::runtime_error("failed to get page size");

    size_t pages = (size + page_size - 1) / page_size;
    size_t alloc_size = pages * static_cast<size_t>(page_size);

    void* ptr = mmap(nullptr, alloc_size,
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
                     -1, 0);

    if (ptr == MAP_FAILED) {
        throw std::runtime_error(std::string("mmap failed: ") + strerror(errno));
    }

    backing_ = static_cast<uint8_t*>(ptr);
    capacity_ = alloc_size;
}

Memory::~Memory() {
    if (backing_ && capacity_ > 0) {
        munmap(static_cast<void*>(backing_), capacity_);
        backing_ = nullptr;
        capacity_ = 0;
    }
}

uint64_t Memory::read(uint64_t addr, int size_bytes) const {
    // if (size_bytes <= 0 || size_bytes > 8)
    //     throw std::invalid_argument("invalid read size");

    uint64_t end = addr + static_cast<uint64_t>(size_bytes);
    // if (end > capacity_)
    //     throw std::out_of_range("memory read out of range");

    uint64_t value = 0;
    std::memcpy(&value, backing_ + static_cast<size_t>(addr), static_cast<size_t>(size_bytes));
    return value;
}

void Memory::write(uint64_t addr, uint64_t value, int size_bytes) {
    // if (size_bytes <= 0 || size_bytes > 8)
    //     throw std::invalid_argument("invalid write size");

    // std::cerr << "Memory::write addr=0x" << std::hex << addr << " value=0x" << value << " size_bytes=" << std::dec << size_bytes << std::endl;

    uint64_t end = addr + static_cast<uint64_t>(size_bytes);
    // if (end > capacity_)
    //     throw std::out_of_range("memory write out of range");

    std::memcpy(backing_ + static_cast<size_t>(addr), &value, static_cast<size_t>(size_bytes));
}

void Memory::load_data(uint64_t addr, const uint8_t* data, size_t size) {
    uint64_t end = addr + static_cast<uint64_t>(size);
    if (end > capacity_)
        throw std::out_of_range("memory load_data out of range");
    std::memcpy(backing_ + static_cast<size_t>(addr), data, size);
}

void Memory::zero_init(uint64_t addr, size_t size) {
    uint64_t end = addr + static_cast<uint64_t>(size);
    if (end > capacity_)
        throw std::out_of_range("memory zero_init out of range");
    std::memset(backing_ + static_cast<size_t>(addr), 0, size);
}

size_t Memory::size() const {
    return capacity_;
}

uint8_t* Memory::data() {
    return backing_;
}

const uint8_t* Memory::data() const {
    return backing_;
}