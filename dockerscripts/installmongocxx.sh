#!/bin/sh

wget https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.6/mongo-cxx-driver-r3.6.6.tar.gz
tar -xzf mongo-cxx-driver-r3.6.6.tar.gz
rm mongo-cxx-driver-r3.6.6.tar.gz
cd mongo-cxx-driver-r3.6.6/build

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBSONCXX_POLY_USE_MNMLSTC=1
cmake --build . --target EP_mnmlstc_core
make -j4
cmake --build . --target install