//
// Created by denis on 5.10.2020.
//

#ifndef LINUXHELLO_SUB_COMMAND_H
#define LINUXHELLO_SUB_COMMAND_H

#include <string>
#include <vector>

#include "positional_argument.h"
class sub_command {



   public:

    std::vector<positional_argument> v_positional;

    bool fired = false;
    std::string m_id;

    void (*func)();

    std::vector<std::string> synonyms;
    sub_command& synonym(const std::string& synonym);
    sub_command& function(void pFunction(void));
    sub_command& flag();
    sub_command& positional(positional_argument pos_arg);
    sub_command& id(const std::string& id);
    void execute();
    void parse(const std::vector<std::string>& string_argv);

    //App& sub_command();

    //void parse(int argc, char* argv[]);


};

#endif  // LINUXHELLO_SUB_COMMAND_H
