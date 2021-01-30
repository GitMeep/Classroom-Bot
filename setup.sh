#!/bin/sh
sudo apt update
sudo apt install -y libssl-dev zlib1g libpq-dev libcurl4-openssl-dev libcurl4 automake libtool make g++ g++-9 libcrypto++-dev libboost-all-dev libcrypto++-dev libfmt-dev wget cmake libssl-dev libsasl2-dev gdb curl

# download submodules (aegis.cpp and restclient-cpp)
git submodule update --init --recursive

# aegis
cd submodules/aegis
sudo ./install-deps.sh
if [ ! -d "./build/" ]; then
  mkdir build
fi
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
mkdir vendor
cd vendor
./downgrade_libss.sh

# libmongoc
if [ ! -d "./mongo/" ]; then
  mkdir mongo
fi
cd mongo
if [ ! -d "./mongo-c-driver-1.17.2/" ]; then
  wget https://github.com/mongodb/mongo-c-driver/releases/download/1.17.2/mongo-c-driver-1.17.2.tar.gz
  tar -xzf mongo-c-driver-1.17.2.tar.gz
  rm mongo-c-driver-1.17.2.tar.gz
else
  echo "\e[92mlibmongoc directory already exists, not downloading\e[39m"
fi
cd mongo-c-driver-1.17.2

if [ ! -d "./cmake-build/" ]; then
  mkdir cmake-build
fi
cd cmake-build
cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -DENABLE_TESTS=OFF -DENABLE_EXAMPLES=OFF ..
make -j4
sudo cmake --build . --target install

# mongocxx
cd ../..
if [ ! -d "./mongo-cxx-driver-r3.6.1/" ]; then 
  curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.1/mongo-cxx-driver-r3.6.1.tar.gz
  tar -xzf mongo-cxx-driver-r3.6.1.tar.gz
  rm mongo-cxx-driver-r3.6.1.tar.gz
else
  echo "\e[92mmongocxx directory already exists, not downloading\e[39m"
fi
cd mongo-cxx-driver-r3.6.1/build

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBSONCXX_POLY_USE_MNMLSTC=1
sudo cmake --build . --target EP_mnmlstc_core
make -j4
sudo cmake --build . --target install

cd ../../../../
./premake.sh