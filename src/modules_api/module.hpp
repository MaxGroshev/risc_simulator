#ifndef MODULE_HPP
#define MODULE_HPP

#include "decode_execute_module/common.hpp"

class Hart;

class Module {
public:
    virtual ~Module() = default;

    // Modules should override this to register their callbacks with a Hart.
    virtual void register_callbacks(Hart& hart) {}
};

#endif // MODULE_HPP