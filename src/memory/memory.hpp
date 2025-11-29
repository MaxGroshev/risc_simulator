#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <cstddef>

class Memory {
public:
    explicit Memory(size_t size = 16ULL * 1024ULL * 1024ULL * 1024ULL);

    Memory(const Memory&) = delete;
    Memory(Memory&&) = delete;
    Memory& operator=(const Memory&) = delete;
    Memory& operator=(Memory&&) = delete;

    ~Memory();

    uint64_t read(uint64_t addr, int size_bytes, bool sign_extend) const;
    void write(uint64_t addr, uint64_t value, int size_bytes);

    void load_data(uint64_t start_addr, const uint8_t* data, size_t size);
    void zero_init(uint64_t start_addr, size_t size);

    size_t size() const;

private:
    uint8_t* backing_ = nullptr;
    size_t capacity_ = 0;
};

#endif // MEMORY_HPP