#ifndef LINUXHELLO_CHECKPOINT_H
#define LINUXHELLO_CHECKPOINT_H

#include <chrono>
#include <map>
#include <string>

#define GET_TIME std::chrono::high_resolution_clock::now()

typedef std::chrono::time_point<std::chrono::high_resolution_clock> time_var;

class checkpoint {
   public:
    checkpoint() = delete;

    enum checkpoint_type { start, progress, finish };

    std::string name;
    time_var time;

    checkpoint_type c_type;

    explicit checkpoint(checkpoint_type);
};

#endif  // LINUXHELLO_CHECKPOINT_H
