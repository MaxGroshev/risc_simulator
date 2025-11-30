#pragma once

#include <fstream>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <filesystem>
#include <iostream>
#include <string>

//-----------------------------------------------------------------------------------------

namespace fs = std::filesystem;

//-----------------------------------------------------------------------------------------

class sim_config_t final {

    public:
        bool   use_jit {0};
        size_t bb_cache_size = {0};
        size_t cached_bb_size {0};
        size_t initial_pc {0};
        size_t read_delay {0};
        size_t initial_reg_val {0};
        size_t max_cycles {0};
        size_t jit_bound {10};

    public:
        sim_config_t() {};
        sim_config_t(fs::path config_file) {
            std::ifstream config_data;
            config_data.open(config_file);
            if (!config_data.good()) {
                throw std::runtime_error(std::string{"Config file does not exist: " +
                                         std::string{config_file}});
            }

            std::string data {};
            size_t pos = std::string::npos;
            while(std::getline(config_data, data)) {
                if (std::string::npos != (pos = data.find("bb_cache_size="))) {
                    std::string str = data.substr(strlen("bb_cache_size="));
                    bb_cache_size = std::stoll(str);
                }
                else if (std::string::npos != (pos = data.find("use_jit="))) {
                    std::string str = data.substr(strlen("use_jit="));
                    use_jit = std::stoi(str.c_str());
                }
                else if (std::string::npos != (pos = data.find("cached_bb_size="))) {
                    std::string str = data.substr(strlen("cached_bb_size="));
                    cached_bb_size = std::stoll(str);
                }
                else if (std::string::npos != (pos = data.find("initial_pc="))) {
                    std::string str = data.substr(strlen("initial_pc="));
                    initial_pc = std::stoll(str);
                }
                else if (std::string::npos != (pos = data.find("read_delay="))) {
                    std::string str = data.substr(strlen("read_delay="));
                    read_delay = std::stoll(str);
                }
                else if (std::string::npos != (pos = data.find("initial_reg_val="))) {
                    std::string str = data.substr(strlen("initial_reg_val="));
                    initial_reg_val = std::stoll(str);
                }
                else if (std::string::npos != (pos = data.find("max_cycles="))) {
                    std::string str = data.substr(strlen("max_cycles="));
                    max_cycles = std::stoll(str);
                }
                else if (std::string::npos != (pos = data.find("jit_bound="))) {
                    std::string str = data.substr(strlen("jit_bound="));
                    jit_bound = std::stoll(str);
                }
            }
            config_data.close();
        }

};
