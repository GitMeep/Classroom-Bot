#!/bin/bash

wget https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.7/mongo-cxx-driver-r3.6.7.tar.gz
tar -xzf mongo-cxx-driver-r3.6.7.tar.gz
rm mongo-cxx-driver-r3.6.7.tar.gz
cd mongo-cxx-driver-r3.6.7/build

# rip out the f'ing tests because the cmake options are not implemented properly!!!!
sed -i '207d' ../src/bsoncxx/CMakeLists.txt
sed -i '234d' ../src/mongocxx/CMakeLists.txt

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_CXX_STANDARD=17 -DENABLE_TESTS=OFF -DBUILD_TESTING=OFF ..
cmake --build . -j7
cmake --build . --target install