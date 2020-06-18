#!/bin/sh

./premake.sh
make -j8 config=release

sudo heroku container:login
sudo heroku container:push worker --app questions-bot
sudo heroku container:release worker --app questions-bot