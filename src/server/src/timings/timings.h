#ifndef TIMINGS_H
#define TIMINGS_H

#include <algorithm>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "record.h"


class timings {
    std::vector<record *> *records;

    timings();

    time_var program_start;

   public:
    timings(timings const &) = delete;
    void operator=(timings const&) = delete;

    static timings* instance();

    record* add_record(std::string name = "");

    record* get_record(std::string name);

    void print();

    std::vector<record*> *get_all_record();
};

#endif /* TIMINGS_H */
