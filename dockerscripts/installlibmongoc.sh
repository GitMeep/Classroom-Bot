#!/bin/bash

wget https://github.com/mongodb/mongo-c-driver/releases/download/1.20.0/mongo-c-driver-1.20.0.tar.gz
tar -xzf mongo-c-driver-1.20.0.tar.gz
rm mongo-c-driver-1.20.0.tar.gz
cd mongo-c-driver-1.20.0

mkdir cmake-build
cd cmake-build
cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -DENABLE_TESTS=OFF -DENABLE_EXAMPLES=OFF ..
make -j7
cmake --build . --target install
cd ../..