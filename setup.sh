#!/bin/sh
sudo apt update
sudo apt install -y openssl libssl-dev zlib1g make g++ wget cmake libspdlog-dev libcrypto++8 libcurl4 libicu67 libssl1.1 libspdlog1

# clone dependencies (poco)
mkdir deps
cd deps

# dpp
git clone --recursive https://github.com/brainboxdotcc/DPP.git
cd DPPcd 
if [ ! -d "./build/" ]; then
  mkdir build
fi
cd build
cmake ..
make -j
sudo make install
cd ../..

# poco
git clone https://github.com/pocoproject/poco.git
cd ./poco
if [ ! -d "./cmake-build/" ]; then
  mkdir cmake-build
fi
cd cmake-build
cmake ..
sudo cmake --build . --target install -j
cd ../..


# libmongoc
if [ ! -d "./mongo/" ]; then
  mkdir mongo
fi
cd mongo
if [ ! -d "./mongo-c-driver-1.20.0/" ]; then
  wget https://github.com/mongodb/mongo-c-driver/releases/download/1.20.0/mongo-c-driver-1.20.0.tar.gz
  tar -xzf mongo-c-driver-1.20.0.tar.gz
  rm mongo-c-driver-1.20.0
else
  echo "\e[92mlibmongoc directory already exists, not downloading\e[39m"
fi
cd mongo-c-driver-1.20.0

if [ ! -d "./cmake-build/" ]; then
  mkdir cmake-build
fi
cd cmake-build
cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -DENABLE_TESTS=OFF -DENABLE_EXAMPLES=OFF ..
make -j
sudo cmake --build . --target install
cd ../..

# mongocxx
if [ ! -d "./mongo-cxx-driver-r3.6.6/" ]; then 
  wget https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.6/mongo-cxx-driver-r3.6.6.tar.gz
  tar -xzf mongo-cxx-driver-r3.6.6.tar.gz
  rm mongo-cxx-driver-r3.6.6.tar.gz
else
  echo "\e[92mmongocxx directory already exists, not downloading\e[39m"
fi
cd mongo-cxx-driver-r3.6.6/build

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBSONCXX_POLY_USE_MNMLSTC=1
sudo cmake --build . --target EP_mnmlstc_core
make -j
sudo cmake --build . --target install

cd ../../../../
curl https://cli-assets.heroku.com/install-ubuntu.sh | sh

./premake.sh