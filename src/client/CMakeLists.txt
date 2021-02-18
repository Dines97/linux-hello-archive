set(CMAKE_SHARED_MODULE_PREFIX "")
file(GLOB Sources src/*.cpp src/*.h)
add_library(pam_linux_hello MODULE ${Sources})
target_link_libraries(pam_linux_hello pam)