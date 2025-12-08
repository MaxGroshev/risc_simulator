#pragma once
#include <string>
#include <cstdint>

enum class PrivilegeMode {
    M,
    S,
    U
};

enum class ExceptionCause{
    None,
    UnknowInstruction,
    PageFault,
};

enum class AccessType : uint64_t {
    Fetch,
    Load,
    Store
};

struct Exception {
    ExceptionCause value;

public:
    Exception(ExceptionCause cause) : value(cause) {};

    std::string to_string() const {
        switch(value) {
            case ExceptionCause::None: return "None";
            case ExceptionCause::UnknowInstruction: return "UnknowInstruction";
            case ExceptionCause::PageFault: return "PageFault";
            default: return "Unknown";
        }
    }

    bool is_none() const {
    return value == ExceptionCause::None;
    }
};