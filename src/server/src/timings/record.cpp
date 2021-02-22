#include "record.h"

#include <iostream>
#include <utility>

record *record::instance() {
    static auto *instance = new record();
    return instance;
}

record::record() {
    this->name = "Master record";
    this->program_start = GET_TIME;

    this->sub_records = new std::vector<record *>;
    this->checkpoints = new std::vector<checkpoint *>;

    auto *new_checkpoint = new checkpoint(checkpoint::checkpoint_type::start);

    this->checkpoints->push_back(new_checkpoint);
}

record::record(std::string name) {
    this->name = std::move(name);
    this->sub_records = new std::vector<record *>;
    this->checkpoints = new std::vector<checkpoint *>;

    auto *new_checkpoint = new checkpoint(checkpoint::checkpoint_type::start);

    this->checkpoints->push_back(new_checkpoint);
}

void record::finish() {
    auto *new_checkpoint = new checkpoint(checkpoint::checkpoint_type::finish);
    this->checkpoints->push_back(new_checkpoint);

    for (record *r : *sub_records) {
        r->finish();
    }
}

void record::print(int indent_size) { print(program_start, indent_size, 0); }

void record::print(time_var time, int size, int indent) {
    static std::map<checkpoint::checkpoint_type, std::string> type_namings;

    type_namings[checkpoint::checkpoint_type::start] = "Start";
    type_namings[checkpoint::checkpoint_type::progress] = "Progress";
    type_namings[checkpoint::checkpoint_type::finish] = "Finish";

    std::cout << std::string(size * indent, ' ') << name << ": ";

    checkpoint *start = get_checkpoint(checkpoint::start);
    checkpoint *finish = get_checkpoint(checkpoint::finish);

    if (start != nullptr && finish != nullptr) {
        auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(finish->time - start->time);
        auto ms = milli.count();

        std::cout << ms << " ms";
    }

    std::cout << std::endl;

    for (record *r : *sub_records) {
        r->print(time, size, indent + 1);
    }

    // std::cout<<checkpoints->size();

    std::cout << std::flush;
}

void record::add_checkpoint() {
    auto *new_checkpoint = new checkpoint(checkpoint::checkpoint_type::progress);

    checkpoints->push_back(new_checkpoint);
}

checkpoint *record::get_checkpoint(checkpoint::checkpoint_type type) {
    for (checkpoint *c : *checkpoints) {
        if (c->c_type == type) {
            return c;
        }
    }

    return nullptr;
}

record *record::get_subrecord(const std::string &n) {
    for (record *r : *sub_records) {
        if (r->get_name() == n) {
            return r;
        }
    }

    throw std::invalid_argument("Cannot find record");
}

record *record::add_subrecord(std::string n) {
    auto *new_record = new record(std::move(n));
    sub_records->push_back(new_record);
    return new_record;
}
std::vector<record *> *record::get_all_subrecord() { return sub_records; }
