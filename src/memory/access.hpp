#pragma once
#include <cstdint>
#include "hart/hart_common.hpp"

// Memory related basic types
using va_t = reg_t;
using pa_t = reg_t;

enum class AccessType {
    Fetch,
    Load,
    Store
};
