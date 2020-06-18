FROM ubuntu:18.04

RUN apt-get update
RUN apt-get install -y libssl-dev zlib1g libpq-dev libcurl4-openssl-dev git libcurl4 automake libtool make g++ g++-9

WORKDIR /usr/src/
RUN git clone --branch 0.5.2 https://github.com/mrtazz/restclient-cpp.git
WORKDIR /usr/src/restclient-cpp/
RUN ./autogen.sh
RUN ./configure
RUN make install
WORKDIR /usr/src/
RUN rm -rf ./restclient-cpp/

WORKDIR /usr/src/classroombot

COPY ./bin/classroombot .
COPY ./default_config.json ./config.json

CMD LD_LIBRARY_PATH=/usr/local/lib ./classroombot