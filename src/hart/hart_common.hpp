#pragma once
#include <string>

enum class PrivilegeMode {
    M,
    S,
    U
};

enum class ExceptionCause{
    None,

};

struct ExceptionCauseWrapper {
    ExceptionCause value;

    std::string to_string() {
        switch(value) {
            case ExceptionCause::None: return "None";
            default: return "Unknown";
        }
    }
};