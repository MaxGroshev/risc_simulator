#pragma once

#include "memory.hpp"

class MMU {
public:
    explicit MMU(Memory &m) : mem_(m) {}

private:
    Memory &mem_;
}