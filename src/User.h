#ifndef LINUXHELLO_USER_H
#define LINUXHELLO_USER_H

#include <dlib/matrix.h>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <string>
#include <vector>

// TODO: Find better name for this structs and their variables
struct Encoding {
    int camera_index;
    dlib::matrix<double, 128, 1> data;

    template <class Archive>
    void save(Archive &archive) const {
        std::vector<double> tmp;
        tmp.reserve(data.nr());
        for (int i = 0; i < data.nr(); i++) {
            tmp.push_back(data(i, 0));
        }
        archive(CEREAL_NVP(camera_index), CEREAL_NVP(tmp));
    }

    template <class Archive>
    void load(Archive &archive) {
        std::vector<double> tmp;
        archive(CEREAL_NVP(camera_index), CEREAL_NVP(tmp));
        for (int i = 0; i < data.nr(); i++) {
            data(i, 0) = tmp[i];
        }
    }
};

struct UserEncoding {
    int id;
    long unix_time;
    std::string label;
    std::vector<Encoding> encodings;

    template <class Archive>
    void serialize(Archive &archive) {
        archive(CEREAL_NVP(id), cereal::make_nvp("time", unix_time), CEREAL_NVP(label), CEREAL_NVP(encodings));
    }
};

struct User {
    std::string user_name;
    std::vector<UserEncoding> user_encodings;

    template <class Archive>
    void serialize(Archive &archive) {
        archive(CEREAL_NVP(user_name), CEREAL_NVP(user_encodings));
    }
};

#endif  // LINUXHELLO_USER_H
