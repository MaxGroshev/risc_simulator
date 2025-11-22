#pragma once

#include <cstdint>
#include <vector>
#include <cstddef>

class Memory {
public:
    explicit Memory(size_t size = 16 * 1024 * 1024);  // Default 16MB

    // TODO(@ArsenySamoylov) memory shouldn't care about sign.
    uint32_t read(uint32_t addr, int size, bool sign_extend) const;
    void    write(uint32_t addr, uint32_t value, int size);

    // TODO(@ArsenySamoylov) remove this function or rewrite it - taking raw pointer as buffer is unsafe.
    void load_data(uint32_t addr, const uint8_t* data, size_t size);

    void zero_init(uint32_t addr, size_t size);

    size_t size() const;

private:
    std::vector<uint8_t> mem_;
};
