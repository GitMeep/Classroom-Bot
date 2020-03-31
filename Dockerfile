FROM ubuntu

WORKDIR /usr/src/questionsbot

COPY questionsbot .
COPY config.json .

RUN apt-get update
RUN apt-get install libssl-dev zlib1g

CMD ["./questionsbot"]