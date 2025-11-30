#pragma once
#include <string>

enum class PrivilegeMode {
    M,
    S,
    U
};

enum class ExceptionCause{
    None,
    UnknowInstruction,
};

struct Exception {
    ExceptionCause value;

public:
    Exception(ExceptionCause cause) : value(cause) {};

    std::string to_string() const {
        switch(value) {
            case ExceptionCause::None: return "None";
            case ExceptionCause::UnknowInstruction: return "UnknowInstruction";
            default: return "Unknown";
        }
    }

    bool is_none() const {
    return value == ExceptionCause::None;
    }
};