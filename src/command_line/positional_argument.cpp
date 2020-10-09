//
// Created by denis on 7.10.2020.
//

#include "positional_argument.h"
positional_argument& positional_argument::position(int p) {
    this->p = p;
    return *this;
}
positional_argument& positional_argument::id(std::string i) {
    this->i = i;
    return *this;
}
positional_argument& positional_argument::required(bool r) {
    this->r = r;
    return *this;
}
