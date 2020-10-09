//
// Created by denis on 7.10.2020.
//

#ifndef LINUXHELLO_POSITIONAL_ARGUMENT_H
#define LINUXHELLO_POSITIONAL_ARGUMENT_H

#include <boost/any.hpp>
#include <string>
class positional_argument {
   public:
    int p;
    std::string i;
    std::string value;
    bool r = false;

    boost::any v;

    positional_argument& position(int p);
    positional_argument& id(std::string i);
    positional_argument& required(bool r);

    template <class T>
    const T& as() const {
        return boost::any_cast<const T&>(v);
    }

    std::string getstring(){
        return value;
    }
};

#endif  // LINUXHELLO_POSITIONAL_ARGUMENT_H
