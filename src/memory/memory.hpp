#pragma once

#include <cstdint>
#include <cstddef>

class Memory {
public:
    explicit Memory(size_t size = 16ULL * 1024ULL * 1024ULL * 1024ULL);
    ~Memory();

    uint64_t read(uint64_t addr, int size_bytes) const;
    void write(uint64_t addr, uint64_t value, int size_bytes);

    // TODO(@ArsenySamoylov) remove this function or rewrite it - taking raw pointer as buffer is unsafe.
    void load_data(uint64_t start_addr, const uint8_t* data, size_t size);
    void zero_init(uint64_t start_addr, size_t size);

    size_t size() const;

    uint8_t* data();
    const uint8_t* data() const;

private:
    uint8_t* backing_ = nullptr;
    size_t capacity_ = 0;
};
