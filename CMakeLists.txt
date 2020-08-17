cmake_minimum_required(VERSION 3.16)
project(LinuxHello)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(PAM REQUIRED)
find_package(OpenCV REQUIRED)
find_package(dlib REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

set(CMAKE_SHARED_MODULE_PREFIX "")
add_library(pam_linux_hello MODULE src/pam_main.cpp)
target_link_libraries(pam_linux_hello pam)

add_executable(linux-hello src/main.cpp src/face_recognition.h src/face_recognition_model_v1.h src/snapshot.h src/darkness.h src/darkness.cpp src/face_recognition_model_v1.cpp src/snapshot.cpp src/face_recognition.cpp)
target_link_libraries(linux-hello pam ${OpenCV_LIBS} dlib::dlib Boost::program_options)

install(TARGETS pam_linux_hello DESTINATION /lib/x86_64-linux-gnu/security/)
install(FILES linux_hello DESTINATION /usr/share/pam-configs/)
install(FILES linux_hello.cfg DESTINATION /etc/linux-hello/)
install(FILES config.json DESTINATION /etc/linux-hello/)
install(DIRECTORY data DESTINATION /etc/linux-hello)
install(DIRECTORY DESTINATION /etc/linux-hello/models)
install(TARGETS linux-hello DESTINATION /usr/bin/)

