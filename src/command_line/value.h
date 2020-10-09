//
// Created by denis on 7.10.2020.
//

#ifndef LINUXHELLO_VALUE_H
#define LINUXHELLO_VALUE_H

#include <boost/any.hpp>
class value {
    boost::any v;

    template <class T>
    const T& as() const {
        return boost::any_cast<const T&>(v);
    }
};

#endif  // LINUXHELLO_VALUE_H
