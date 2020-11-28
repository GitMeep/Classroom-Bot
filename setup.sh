#!/bin/sh

sudo apt-get update
sudo apt-get install -y libssl-dev zlib1g libpq-dev libcurl4-openssl-dev libcurl4 automake libtool make g++ g++-9 libcrypto++-dev libboost-all-dev libcrypto++-dev libfmt-dev wget cmake libssl-dev libsasl2-dev gdb

# download submodules (aegis.cpp and restclient-cpp)
git submodule update --init --recursive

# aegis
cd submodules/aegis
sudo ./install-deps.sh
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=g++-9 -DCMAKE_CXX_STANDARD=17 ..
make -j3
sudo make install

cd ../..

# restclient-cpp
cd restclient-cpp
./autogen.sh
./autoconfigure
sudo make install

# downgrade libssl
cd ../..
mkdir lib
cd lib
rm libssl-dev_1.1.1-1ubuntu2.1~18.04.6_amd64.deb
rm libssl1.1_1.1.1-1ubuntu2.1~18.04.6_amd64.deb
wget http://security.ubuntu.com/ubuntu/pool/main/o/openssl/libssl-dev_1.1.1-1ubuntu2.1~18.04.6_amd64.deb
wget http://security.ubuntu.com/ubuntu/pool/main/o/openssl/libssl1.1_1.1.1-1ubuntu2.1~18.04.6_amd64.deb
sudo dpkg -i libssl-dev_1.1.1-1ubuntu2.1~18.04.6_amd64.deb libssl1.1_1.1.1-1ubuntu2.1~18.04.6_amd64.deb

# libmongoc
mkdir mongo
cd mongo
wget https://github.com/mongodb/mongo-c-driver/releases/download/1.17.2/mongo-c-driver-1.17.2.tar.gz
tar -xzf mongo-c-driver-1.17.2.tar.gz
rm mongo-c-driver-1.17.2.tar.gz
cd mongo-c-driver-1.17.2
mkdir cmake-build
cd cmake-build
cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF ..
make -j4
sudo cmake --build . --target install

# mongocxx
cd ../..
curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.1/mongo-cxx-driver-r3.6.1.tar.gz
tar -xzf mongo-cxx-driver-r3.6.1.tar.gz
rm mongo-cxx-driver-r3.6.1.tar.gz
cd mongo-cxx-driver-r3.6.1/build

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBSONCXX_POLY_USE_MNMLSTC=1
sudo cmake --build . --target EP_mnmlstc_core
make -j4
sudo cmake --build . --target install

cd ../../../..

