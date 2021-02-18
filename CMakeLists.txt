cmake_minimum_required(VERSION 3.16)
project(LinuxHello)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_subdirectory(src)

install(TARGETS pam_linux_hello DESTINATION /lib/x86_64-linux-gnu/security/)
install(FILES linux_hello DESTINATION /usr/share/pam-configs/)
install(FILES config.toml DESTINATION /etc/linux-hello/)
install(DIRECTORY data DESTINATION /etc/linux-hello)
install(DIRECTORY DESTINATION /etc/linux-hello/models)
install(TARGETS linux-hello DESTINATION /usr/bin/)

set(CPACK_PACKAGE_NAME "linux-hello")
set(CPACK_PACKAGE_VERSION 0.1.0)
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 1)
set(CPACK_PACKAGE_VERSION_PATCH 0)

#Debian
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Denis Kaynar")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libdlib19, libboost-program-options1.71.0, libopencv-highgui4.2")

include(CPack)
