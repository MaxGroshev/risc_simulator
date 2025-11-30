#pragma once

#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <iostream>
#include <set>
#include <boost/program_options.hpp>

//-----------------------------------------------------------------------------------------

namespace fs = std::filesystem;

//-----------------------------------------------------------------------------------------

struct prog_config_t final {
    private:
        const std::set<std::string> allowed_modules = {"example"};

    public:

        fs::path input_file;
        fs::path stat_file;
        std::optional<std::string> module_name = std::nullopt;
        fs::path config_file = "../config/configx86.conf";

        boost::program_options::options_description set_option_description() {
            using namespace boost::program_options;
            options_description desc{"Options"};
            desc.add_options()
            ("help,h", "help screen")
            ("input,i",  value<std::string>(), "source file")
            ("config,c",  value<std::string>(), "configuration file")
            ("output,o",value<std::string>(), "statistic dump file")
            ("module,m",value<std::string>(), "module");

            return desc;
        }

    public:
        prog_config_t() {};
        prog_config_t(const int argc, char** argv) {
            using namespace boost::program_options;

            options_description desc = set_option_description() ;

            variables_map args;
            store(parse_command_line(argc, argv, desc), args);
            notify(args);

            if (args.count("help")) {
                std::cout << desc << '\n';
            }
            if (args.count("config")) {
                config_file = std::move(args["config"].as<std::string>());
            }
            if (args.count("module")) {
                if (allowed_modules.find(args["module"].as<std::string>()) != allowed_modules.end()) {
                    module_name = std::move(args["module"].as<std::string>());
                } else {
                    throw std::runtime_error("Unexpecte module name: " + args["module"].as<std::string>() + "\n");
                }
            }
            if (args.count("input")) {
                input_file = std::move(args["input"].as<std::string>());
            }
            else
                throw std::runtime_error("No source file is listed\n");
            if (args.count("output"))
                stat_file = std::move(args["output"].as<std::string>());
        }
};
