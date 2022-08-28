#!/bin/bash

git clone --recursive https://github.com/brainboxdotcc/DPP.git
cd DPP # ./deps/DPP
git checkout v10.0.16
cmake -B ./build
cmake --build ./build -j7
cd build # ./deps/DPP/build
make install
cd ../.. # ./deps
