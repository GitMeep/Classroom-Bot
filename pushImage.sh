#!/bin/sh

if test -z "$1"
then
    echo "Usage: ./pushImage.sh [version]"
else
    sudo docker login
    sudo docker push meepdocker/classroom-bot:$1
fi