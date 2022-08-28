#!/bin/bash

if test -z "$1"
then
    echo "Usage: ./buildImage.sh [version]"
else
    sudo docker build -t meepdocker/classroom-bot:latest -t meepdocker/classroom-bot:$1 .
fi