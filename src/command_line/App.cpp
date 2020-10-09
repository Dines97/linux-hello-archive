#include "App.h"

#include <iostream>
App& App::add_sub_command(const sub_command& sub_cmd) {
    sub_commands.push_back(sub_cmd);
    return *this;
}
void App::parse(int argc, char** argv) {

    path = argv[0];

    std::vector<std::string> string_argv;
    string_argv.reserve(argc-1);
    for (int i = 1; i < argc; ++i) {
        string_argv.emplace_back(argv[i]);
    }

    std::cout<<path<<std::endl;

    /*std::cout << string_argv.size() << std::endl;
    for (auto& i : string_argv) {
        std::cout << i << std::endl;
    }*/

    for (auto& i : string_argv) {
        if (i[0] == '-') {
            // For flags
        } else {
            // For subcommand
            for (auto& sub_command : sub_commands) {
                for (auto& synonym : sub_command.synonyms) {
                    if (synonym == i) {
                        sub_command.fired = true;

                        string_argv.erase(string_argv.begin(),i)

                        sub_command.parse();

                        return;
                    }
                }
            }
        }
    }
}

bool App::sub_command_matches(const std::string& id) {
    for (auto& sub_command : sub_commands) {
        if (sub_command.m_id == id) return true;
    }
    return false;
}
positional_argument App::get(std::string id) {
    for (auto& sub_command : sub_commands) {
        for (auto& j : sub_command.v_positional) {
            if (j.i == id) {
                return j;
            }
        }
    }
}
