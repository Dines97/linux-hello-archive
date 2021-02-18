#include "record.h"

#include <iostream>
#include <utility>

record::record() { checkpoints = new std::vector<checkpoint *>; }

record::record(std::string name) {
    this->name = std::move(name);
    this->checkpoints = new std::vector<checkpoint *>;

    auto *new_checkpoint = new checkpoint(checkpoint::checkpoint_type::start);

    this->checkpoints->push_back(new_checkpoint);
}

void record::finish() {
    auto *new_checkpoint = new checkpoint(checkpoint::checkpoint_type::finish);
    this->checkpoints->push_back(new_checkpoint);
}

void record::print(time_var time) {
    static std::map<checkpoint::checkpoint_type, std::string> type_namings;

    type_namings[checkpoint::checkpoint_type::start] = "Start";
    type_namings[checkpoint::checkpoint_type::progress] = "Progress";
    type_namings[checkpoint::checkpoint_type::finish] = "Finish";

    std::cout << name << ": ";

    checkpoint *start = get_checkpoint(checkpoint::start);
    checkpoint *finish = get_checkpoint(checkpoint::finish);

    if (start != nullptr && finish != nullptr) {
        auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(finish->time - start->time);
        auto ms = milli.count();

        std::cout << ms << " ms";
    }

    std::cout << "\n";

    // std::cout<<checkpoints->size();

    for (checkpoint *c_point : *checkpoints) {
        auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(c_point->time - time);

        std::cout << "\t" << type_namings[c_point->c_type] <<" at: "<< milli.count() << "\n";
    }

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
