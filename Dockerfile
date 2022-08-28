FROM ubuntu:22.04 as builder

ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /deps/
RUN apt update && apt install -y git openssl libssl-dev zlib1g-dev make g++ clang wget cmake libcrypto++-dev libcurl4 libicu70 libssl3 libspdlog-dev libfmt-dev libpoco-dev libmongoc-dev libbson-dev
COPY ./dockerscripts/installmongocxx.sh ./scripts/installmongocxx.sh
RUN ./scripts/installmongocxx.sh
COPY ./dockerscripts/installdpp.sh ./scripts/installdpp.sh
RUN ./scripts/installdpp.sh
WORKDIR /classroombot/
COPY ./premake/ ./premake/
COPY ./src/ ./src/
COPY ./premake5.lua ./premake.sh ./
RUN ./premake.sh && make -j7 config=release

FROM ubuntu:22.04 as image
WORKDIR /classroombot/
RUN apt update && apt install -y libcrypto++8 libcurl4 libicu70 openssl libssl-dev libfmt8 libmongoc-1.0-0
COPY ./default_config.json ./config.json
COPY ./lang ./lang
COPY --from=builder /usr/local/lib/ /usr/local/lib/
COPY --from=builder /usr/lib/ /usr/lib/
COPY --from=builder /classroombot/bin/classroombot .

ENV LD_LIBRARY_PATH=/usr/local/lib:/usr/lib/

CMD ./classroombot