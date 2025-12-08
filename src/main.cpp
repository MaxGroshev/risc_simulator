#include "machine/machine.hpp"
#include "modules/example_module.hpp"
#include <iostream>
#include <cstdint>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <elf_file> [max_cycles] [module1 module2 ...]" << std::endl;
        return 1;
    }

    uint64_t max_cycles = (argc > 2) ? std::stoull(argv[2]) : 0;

    std::vector<std::string> modules;
    for (int i = 3; i < argc; ++i) {
        modules.push_back(argv[i]);
    }

    Machine machine;

    try {
        machine.load_elf(argv[1]);

        for (const auto& mod_name : modules) {
            if (mod_name == "example") {
                auto ex = std::make_shared<ExampleModule>();
                ex->register_callbacks(machine.get_hart());
                machine.get_hart().add_module(ex);
            } else {
                std::cerr << "Unknown module: " << mod_name << std::endl;
            }
        }
        
        machine.run(max_cycles);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}