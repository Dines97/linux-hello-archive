#ifndef LINUXHELLO_APP_H
#define LINUXHELLO_APP_H

#include <vector>

#include "sub_command.h"
class App {
    std::vector<sub_command> sub_commands;
    std::string path;

   public:

    App& positional();
    App& option();
    App& flag();

    App& add_sub_command(const sub_command& sub_cmd);

    void parse(int argc, char* argv[]);

    bool sub_command_matches(const std::string& id);

    positional_argument get(std::string id);

};

#endif  // LINUXHELLO_APP_H
