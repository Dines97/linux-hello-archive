#ifndef LINUXHELLO_RECORD_H
#define LINUXHELLO_RECORD_H

#include <vector>

#include "checkpoint.h"

class record {
    time_var program_start;

    std::string name;

    std::vector<record *> *sub_records;
    std::vector<checkpoint *> *checkpoints;

    record();
    explicit record(std::string);

   public:
    record(record const &) = delete;
    void operator=(record const &) = delete;

    static record *instance();

    void add_checkpoint();
    record *add_subrecord(std::string);
    record *get_subrecord(const std::string &);
    std::vector<record *> *get_all_subrecord();
    void finish();

    void print(int indent_size);
    void print(time_var, int size, int);

    std::string get_name() { return name; }
    checkpoint *get_checkpoint(checkpoint::checkpoint_type);
};

#endif  // LINUXHELLO_RECORD_H
