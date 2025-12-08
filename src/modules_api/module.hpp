#ifndef MODULE_HPP
#define MODULE_HPP

#include "modules_api/callbacks.hpp"

class Hart;

class Module {
public:
    virtual ~Module() = default;

    // Modules should override this to register their callbacks with a Hart.
    virtual void register_callbacks(Hart& hart) {}
};

#endif // MODULE_HPP