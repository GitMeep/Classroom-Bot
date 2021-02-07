FROM ubuntu:20.04 as builder

ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /src/
RUN apt update && apt install -y git openssl libssl-dev zlib1g libpq-dev libcurl4-openssl-dev libcurl4 automake libtool make g++ g++-9 libcrypto++-dev libboost-all-dev libcrypto++-dev libfmt-dev wget cmake libssl-dev libsasl2-dev gdb curl
COPY ./dockerbuilder.sh .
COPY ./downgrade_libssl.sh .
RUN ./dockerbuilder.sh

FROM ubuntu:20.04
WORKDIR /classroombot/
COPY --from=builder /usr/local/lib ./libs/
COPY --from=builder ["/lib/x86_64-linux-gnu/libcrypto++.so.6", "./libs/"]
COPY ./docker_runtime_libs.sh .
RUN ./docker_runtime_libs.sh
COPY ./default_config.json ./config.json
COPY ./lang ./lang
COPY ./bin/classroombot .

ENV LD_LIBRARY_PATH=/classroombot/libs/;/lib/x86_64-linux-gnu/
CMD ./classroombot