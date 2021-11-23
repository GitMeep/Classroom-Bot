FROM ubuntu:21.04 as builder

ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /deps/
RUN apt update && apt install -y apt-utils git openssl libssl-dev zlib1g make g++ wget cmake libspdlog-dev
COPY ./dockerscripts/ ./scripts/
RUN ./scripts/installdpp.sh && ./scripts/installpoco.sh && ./scripts/installlibmongoc.sh && ./scripts/installmongocxx.sh
WORKDIR /classroombot/
COPY ./premake/ ./premake/
COPY ./src/ ./src/
COPY ./premake5.lua ./premake.sh ./
RUN ./premake.sh && make -j4 config=release

FROM ubuntu:21.04
WORKDIR /classroombot/
RUN apt update && apt install -y libcrypto++8 libcurl4 libicu67 libssl1.1 libspdlog1
COPY ./default_config.json ./config.json
COPY ./lang ./lang
COPY --from=builder /classroombot/bin/classroombot .

CMD ./classroombot