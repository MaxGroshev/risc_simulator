#include "machine/machine.hpp"
#include "sim_config.hpp"
#include "prog_config.hpp"
#include <iostream>
#include <cstdint>

#ifdef ENABLE_MODULES
#include "modules/example_module.hpp"
#endif

int main(int argc, char* argv[]) {
    
    prog_config_t prog_conf{argc, argv};
    sim_config_t  sim_conf{prog_conf.config_file};
    Machine machine{sim_conf};

    try {
        machine.load_elf(prog_conf.input_file);
#ifdef ENABLE_MODULES
        if (sim_conf.use_jit && prog_conf.module_name.has_value()) {
            std::cout << "Modules are disabled when JIT is enabled. Set use_jit=0 or omit --module." << std::endl;
        }
        else if (prog_conf.module_name.has_value() && 
            prog_conf.module_name.value() == "example") {
                auto ex = std::make_shared<ExampleModule>();
                ex->register_callbacks(machine.get_hart());
                machine.get_hart().add_module(ex);
        }
#endif
        machine.run(sim_conf.max_cycles);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
