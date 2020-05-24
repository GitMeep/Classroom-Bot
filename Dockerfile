FROM ubuntu

WORKDIR /usr/src/classroombot

COPY ./build/classroombot .
COPY ./default_config.json ./config.json

RUN apt-get update
RUN apt-get install -y libssl-dev zlib1g libpq-dev libcurl4-openssl-dev

CMD ./classroombot