#!/bin/sh

sudo apt-get install -y libssl-dev zlib1g libpq-dev libcurl4-openssl-dev libcurl4 automake libtool make g++ g++-9 libcrypto++-dev libboost-all-dev libcrypto++-dev libfmt-dev wget

# download submodules (aegis.cpp and restclient-cpp)
git submodule update --init --recursive

# 
cd submodules/aegis
sudo ./install-deps.sh
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=g++-9 -DCMAKE_CXX_STANDARD=17 ..
make -j3
sudo make install

cd ..

cd restclient-cpp
./autogen.sh
./autoconfigure
sudo make install

# downgrade libssl
cd ../..
mkdir libssl_old
cd libssl_old
wget http://security.ubuntu.com/ubuntu/pool/main/o/openssl/libssl-dev_1.1.1-1ubuntu2.1~18.04.6_amd64.deb
wget http://security.ubuntu.com/ubuntu/pool/main/o/openssl/libssl1.1_1.1.1-1ubuntu2.1~18.04.6_amd64.deb
sudo dpkg -i libssl-dev_1.1.1-1ubuntu2.1~18.04.6_amd64.deb libssl1.1_1.1.1-1ubuntu2.1~18.04.6_amd64.deb