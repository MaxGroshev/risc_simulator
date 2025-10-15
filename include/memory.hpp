#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <vector>
#include <cstddef>

class Memory {
public:
    Memory(size_t size = 16 * 1024 * 1024);  // Default 16MB

    uint32_t read(uint32_t addr, int size, bool sign_extend) const;
    void write(uint32_t addr, uint32_t value, int size);

    // Load data into memory (for ELF segments)
    void load_data(uint32_t addr, const uint8_t* data, size_t size);

    // Zero-init a range (for .bss)
    void zero_init(uint32_t addr, size_t size);

private:
    std::vector<uint8_t> mem_;
};

#endif // MEMORY_HPP