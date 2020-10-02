#ifndef LINUXHELLO_FACE_RECOGNITION_MODEL_V1_H
#define LINUXHELLO_FACE_RECOGNITION_MODEL_V1_H

#include <dlib/dnn.h>

class face_recognition_model_v1 {
   public:
    explicit face_recognition_model_v1(const std::string &model_filename);

    dlib::matrix<double, 0, 1> compute_face_descriptor(const dlib::matrix<dlib::rgb_pixel> &img,
                                                       const dlib::full_object_detection &face, int num_jitters,
                                                       float padding = 0.25);

    std::vector<dlib::matrix<double, 0, 1>> compute_face_descriptors(
        const dlib::matrix<dlib::rgb_pixel> &img, const std::vector<dlib::full_object_detection> &faces,
        int num_jitters, float padding = 0.25);

    std::vector<std::vector<dlib::matrix<double, 0, 1>>> batch_compute_face_descriptors(
        std::vector<dlib::matrix<dlib::rgb_pixel>> batch_imgs,
        const std::vector<std::vector<dlib::full_object_detection>> &batch_faces, int num_jitters,
        float padding = 0.25);

   private:
    dlib::rand rnd;

    std::vector<dlib::matrix<dlib::rgb_pixel>> jitter_image(const dlib::matrix<dlib::rgb_pixel> &img, int num_jitters);

    template <template <int, template <typename> class, int, typename> class block, int N, template <typename> class BN,
              typename SUBNET>
    using residual = dlib::add_prev1<block<N, BN, 1, dlib::tag1<SUBNET>>>;

    template <template <int, template <typename> class, int, typename> class block, int N, template <typename> class BN,
              typename SUBNET>
    using residual_down =
        dlib::add_prev2<dlib::avg_pool<2, 2, 2, 2, dlib::skip1<dlib::tag2<block<N, BN, 2, dlib::tag1<SUBNET>>>>>>;

    template <int N, template <typename> class BN, int stride, typename SUBNET>
    using block = BN<dlib::con<N, 3, 3, 1, 1, dlib::relu<BN<dlib::con<N, 3, 3, stride, stride, SUBNET>>>>>;

    template <int N, typename SUBNET>
    using ares = dlib::relu<residual<block, N, dlib::affine, SUBNET>>;
    template <int N, typename SUBNET>
    using ares_down = dlib::relu<residual_down<block, N, dlib::affine, SUBNET>>;

    template <typename SUBNET>
    using alevel0 = ares_down<256, SUBNET>;
    template <typename SUBNET>
    using alevel1 = ares<256, ares<256, ares_down<256, SUBNET>>>;
    template <typename SUBNET>
    using alevel2 = ares<128, ares<128, ares_down<128, SUBNET>>>;
    template <typename SUBNET>
    using alevel3 = ares<64, ares<64, ares<64, ares_down<64, SUBNET>>>>;
    template <typename SUBNET>
    using alevel4 = ares<32, ares<32, ares<32, SUBNET>>>;

    using anet_type = dlib::loss_metric<dlib::fc_no_bias<
        128,
        dlib::avg_pool_everything<alevel0<alevel1<alevel2<alevel3<alevel4<dlib::max_pool<
            3, 3, 2, 2, dlib::relu<dlib::affine<dlib::con<32, 7, 7, 2, 2, dlib::input_rgb_image_sized<150>>>>>>>>>>>>>;
    anet_type net;
};

#endif  // LINUXHELLO_FACE_RECOGNITION_MODEL_V1_H
