//
// Created by denis on 5.10.2020.
//

#include "sub_command.h"

#include <iostream>

sub_command& sub_command::synonym(const std::string& synonym) {
    for (auto& i : synonyms) {
        if (i == synonym) {
            // Produce error
        }
    }
    synonyms.push_back(synonym);

    return *this;
}

sub_command& sub_command::function(void (*pFunction)()) {
    this->func = pFunction;
    return *this;
}
void sub_command::execute() { (*func)(); }
sub_command& sub_command::id(const std::string& id) {
    m_id = id;
    return *this;
}
sub_command& sub_command::positional(positional_argument pos_arg) {
    v_positional.push_back(pos_arg);
    return *this;
}
void sub_command::parse(const std::vector<std::string>& string_argv) {
    std::cout<<"TEST"<<std::endl;

    std::cout<<argc<<std::endl;

    for (int j = 0; j < argc; ++j) {
        std::cout << argv[j] << std::endl;
    }

    for (int i = 0; i < argc; ++i) {
        for (auto& j : v_positional) {
            if (j.p == i) {
                std::cout << argv[i] << std::endl;
                j.v = argv[i];
                j.value = argv[i];
            }
        }
    }
}
