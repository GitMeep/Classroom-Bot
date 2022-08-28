#!/bin/bash
sudo apt update
sudo apt install -y git openssl libssl-dev zlib1g-dev make g++ clang wget cmake libcrypto++-dev libcurl4 libicu70 libssl3 libspdlog-dev libfmt-dev libmongocrypt-dev

mkdir deps
cd deps # ./deps

# dpp
if [ ! -d "./DPP/" ]; then
  git clone --recursive https://github.com/brainboxdotcc/DPP.git
  cd DPP # ./deps/DPP
  cmake -B ./build
  cmake --build ./build -j7
  cd build # ./deps/DPP/build
  sudo make install
  cd ../.. # ./deps
fi

# poco
git clone https://github.com/pocoproject/poco.git
cd ./poco # ./deps/poco
if [ ! -d "./cmake-build/" ]; then
  mkdir cmake-build
fi
cd cmake-build # ./deps/poco/cmake-build
cmake ..
sudo cmake --build . --target install -j7
cd ../.. # ./deps


# libmongoc
if [ ! -d "./mongo/" ]; then
  mkdir mongo
fi
cd mongo # ./deps/mongo
if [ ! -d "./mongo-c-driver-1.20.0/" ]; then
  wget https://github.com/mongodb/mongo-c-driver/releases/download/1.20.0/mongo-c-driver-1.20.0.tar.gz
  tar -xzf mongo-c-driver-1.20.0.tar.gz
  rm mongo-c-driver-1.20.0.tar.gz
else
  echo "\e[92mlibmongoc directory already exists, not downloading\e[39m"
fi
cd mongo-c-driver-1.20.0 # ./deps/mongo/mongo-c-driver-1.20.0

if [ ! -d "./cmake-build/" ]; then
  mkdir cmake-build
fi
cd cmake-build # ./deps/mongo/mongo-c-driver-1.20.0/cmake-build
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -DENABLE_TESTS=OFF -DENABLE_EXAMPLES=OFF -DENABLE_CLIENT_SIDE_ENCRYPTION=ON ..
cmake --build . -j7
sudo cmake --build . --target install
cd ../.. # ./deps/mongo

# mongocxx
if [ ! -d "./mongo-cxx-driver-r3.6.7/" ]; then 
  wget https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.7/mongo-cxx-driver-r3.6.7.tar.gz
  tar -xzf mongo-cxx-driver-r3.6.7.tar.gz
  rm mongo-cxx-driver-r3.6.7.tar.gz
else
  echo "\e[92mmongocxx directory already exists, not downloading\e[39m"
fi
cd mongo-cxx-driver-r3.6.7/build # ./deps/mongo/mongo-cxx-driver-r3.6.7/build

cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_CXX_STANDARD=17 ..
cmake --build . -j7
sudo cmake --build . --target install

cd ../../../.. # ./
curl https://cli-assets.heroku.com/install-ubuntu.sh | sh

./premake.sh