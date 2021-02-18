#include <utility>

#include "timings.h"

timings::timings() {
    program_start = GET_TIME;
    records = new std::vector<record *>;
}

timings *timings::instance() {
    static auto *instance = new timings();
    return instance;
}

record* timings::add_record(std::string name) {
    auto *new_record = new record(std::move(name));
    records->push_back(new_record);
    return new_record;
}

record *timings::get_record(std::string name) {
    for (record *r : *records) {
        if (r->get_name() == name) {
            return r;
        }
    }

    throw std::invalid_argument("Cannot find record");
}

void timings::print() {
    for (record *r : *records) {
        r->print(program_start);
    }
}

std::vector<record *> *timings::get_all_record() { return records; }
