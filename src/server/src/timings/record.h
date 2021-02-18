#ifndef LINUXHELLO_RECORD_H
#define LINUXHELLO_RECORD_H

#include <vector>

#include "checkpoint.h"

class record {
    std::string name;

    std::vector<checkpoint *> *checkpoints;

   public:
    record();
    explicit record(std::string name = "");
    void add_checkpoint();
    void finish();

    void print(time_var);

    std::string get_name() { return name; }
    checkpoint *get_checkpoint(checkpoint::checkpoint_type);
};

#endif  // LINUXHELLO_RECORD_H
