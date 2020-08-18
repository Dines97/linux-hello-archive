rm -rf build
mkdir build
cd build
cmake ..
cmake --build .
cpack -G "DEB"