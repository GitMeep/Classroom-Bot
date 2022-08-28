#!/bin/bash

git clone https://github.com/pocoproject/poco.git
cd poco
mkdir cmake-build
cd cmake-build
cmake .. -DENABLE_XML=OFF \
         -DENABLE_JSON=OFF \
         -DENABLE_NET=ON \
         -DENABLE_NETSSL=ON \
         -DENABLE_CRYPTO=ON \
         -DENABLE_JWT=OFF \
         -DENABLE_DATA=OFF \
         -DENABLE_DATA_SQLITE=OFF \
         -DENABLE_DATA_MYSQL=OFF \
         -DENABLE_DATA_POSTGRESQL=OFF \
         -DENABLE_DATA_ODBC=OFF \
         -DENABLE_MONGODB=OFF \
         -DENABLE_REDIS=OFF \
         -DENABLE_PDF=OFF \
         -DENABLE_UTIL=OFF \
         -DENABLE_ZIP=OFF \
         -DENABLE_SEVENZIP=OFF \
         -DENABLE_APACHECONNECTOR=OFF \
         -DENABLE_CPPPARSER=OFF \
         -DENABLE_ENCODINGS=OFF \
         -DENABLE_ENCODINGS_COMPILER=OFF \
         -DENABLE_PAGECOMPILER=OFF \
         -DENABLE_PAGECOMPILER_FILE2PAGE=OFF \
         -DENABLE_POCODOC=OFF \
         -DENABLE_TESTS=OFF \
         -DENABLE_LONG_RUNNING_TESTS=OFF \
         -DPOCO_UNBUNDLED=OFF
cmake --build . --target install -j7
cd ../..