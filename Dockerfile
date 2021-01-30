FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y zlib1g libpq-dev libcurl4-openssl-dev libcurl4 libcrypto++-dev libcrypto++-dev libfmt-dev

WORKDIR /usr/src/classroombot
# install older version of libssl, the newer versions cause the bot to be unable to connect to discord
RUN apt-get install -y wget libicu-dev
RUN wget http://mirrors.kernel.org/ubuntu/pool/main/o/openssl/libssl-dev_1.1.1-1ubuntu2.1~18.04.7_amd64.deb http://mirrors.kernel.org/ubuntu/pool/main/o/openssl/libssl1.1_1.1.1-1ubuntu2.1~18.04.7_amd64.deb
RUN dpkg -i libssl-dev_1.1.1-1ubuntu2.1~18.04.7_amd64.deb libssl1.1_1.1.1-1ubuntu2.1~18.04.7_amd64.deb
COPY ./libs/libmongocxx.so /usr/local/lib/libmongocxx.so._noabi
COPY ./libs/libbsoncxx.so /usr/local/lib/libbsoncxx.so._noabi
COPY ./libs/libbson-1.0.so.0 /usr/local/lib/libbson-1.0.so.0
COPY ./libs/libmongoc-1.0.so.0 /usr/local/lib/libmongoc-1.0.so.0
COPY ./bin/classroombot .
COPY ./default_config.json ./config.json

ENV LD_LIBRARY_PATH=/usr/local/lib
CMD ./classroombot