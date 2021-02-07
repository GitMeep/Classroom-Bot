#!/bin/sh

# This is the same file as setup.sh, but with sudo removed and git added to work in the docker build

mkdir vendor
cd vendor
git clone --recursive https://github.com/GitMeep/aegis.cpp.git
git clone https://github.com/pocoproject/poco.git

# aegis
cd aegis.cpp
./install-deps.sh
rm -rf build
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=g++-9 -DCMAKE_CXX_STANDARD=17 ..
make -j4
make install
cd ../..

# poco
cd poco
rm -rf cmake-build
mkdir cmake-build
cd cmake-build
cmake ..
cmake --build . --target install -j4
cd ../..

# libmongoc
rm -rf mongo
mkdir mongo
cd mongo
rm -rf mongo-c-driver-1.17.2
wget https://github.com/mongodb/mongo-c-driver/releases/download/1.17.2/mongo-c-driver-1.17.2.tar.gz
tar -xzf mongo-c-driver-1.17.2.tar.gz
rm mongo-c-driver-1.17.2.tar.gz

cd mongo-c-driver-1.17.2

rm -rf cmake-build
mkdir cmake-build
cd cmake-build
cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -DENABLE_TESTS=OFF -DENABLE_EXAMPLES=OFF ..
make -j4
cmake --build . --target install

# mongocxx
cd ../..
rm -rf mongo-cxx-driver-r3.6.1
curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.1/mongo-cxx-driver-r3.6.1.tar.gz
tar -xzf mongo-cxx-driver-r3.6.1.tar.gz
rm mongo-cxx-driver-r3.6.1.tar.gz
cd mongo-cxx-driver-r3.6.1/build

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBSONCXX_POLY_USE_MNMLSTC=1
cmake --build . --target EP_mnmlstc_core
make -j4
cmake --build . --target install

cd ../../../../
