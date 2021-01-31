#include "timings.h"

checkpoint::checkpoint(checkpoint_type type){
    this->time = GET_TIME;
    this->c_type = type;
}

record::record(std::string name){
    this->name = name;

    checkpoint *new_checkpoint = new checkpoint(checkpoint::checkpoint_type::start);

    std::cout<<"test"<<std::endl;


    this->checkpoints->push_back(new_checkpoint);
}

void record::finish(){

    checkpoint *new_checkpoint = new checkpoint(checkpoint::checkpoint_type::finish);
    this->checkpoints->push_back(new_checkpoint);
}

void record::print(){

    std::cout<<"amk"<<std::endl;

    for(checkpoint *c_point : *checkpoints){
        std::cout<<"ank"<<std::endl;
    }
}


timings& timings::instance(){
    static timings instance;
    return instance;
}


void timings::add_record(std::string name){
    record new_record(name);
    records->push_back(&new_record);
}

record* timings::get_record(std::string name){
    for(record *r : *records){
        if (r->get_name() == name) {
            return r;
        }
    }

    throw std::invalid_argument("Cannot find record");
}

void timings::print(){
    for(record *r : *records){
        r->print();
    }
}

std::vector<record*> *timings::get_all_record(){
    return records;
}
