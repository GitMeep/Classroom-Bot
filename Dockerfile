FROM ubuntu

WORKDIR /usr/src/questionsbot

COPY build/questionsbot .

RUN apt-get update
RUN apt-get install libssl-dev zlib1g

CMD ["./questionsbot"]