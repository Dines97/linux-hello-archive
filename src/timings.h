#ifndef TIMINGS_H
#define TIMINGS_H

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>


#define GET_TIME std::chrono::high_resolution_clock::now();

typedef std::chrono::time_point<std::chrono::high_resolution_clock> time_var;

class checkpoint{

    public:

    enum checkpoint_type{start, finish};


    std::string name;
    time_var time;

    checkpoint_type c_type;

    checkpoint(checkpoint_type);

};

class record{

    std::string name;

    std::vector<checkpoint*> *checkpoints;

    public:

    record();
    record(std::string name);
    void add_checkpoint();
    void finish();

    void print();

    std::string get_name(){
        return name;
    }
};


class timings{
    std::vector<record*> *records;

    timings(){}

   public:

    static timings& instance();

    void add_record(std::string name);

    record* get_record(std::string name);

    void print();

    std::vector<record*> *get_all_record();

};


#endif /* TIMINGS_H */
