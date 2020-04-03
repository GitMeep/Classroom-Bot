FROM ubuntu

WORKDIR /usr/src/classroombot

COPY ./build/classroombot .

RUN apt-get update
RUN apt-get install libssl-dev zlib1g

CMD ./classroombot ${BOT_TOKEN}