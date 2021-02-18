find_package(PAM REQUIRED)
find_package(OpenCV REQUIRED)
find_package(dlib REQUIRED)

file(GLOB_RECURSE Sources src/*.cpp src/*.h)
add_executable(linux-hello ${Sources})
target_link_libraries(linux-hello pam ${OpenCV_LIBS} dlib::dlib)
