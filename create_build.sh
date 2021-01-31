#!/bin/bash

rm  ./compile_commands

rm -rf ./build
mkdir build
cd ./build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=YES ..
mv compile_commands.json ../compile_commands.json

