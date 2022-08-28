#!/bin/bash

sudo heroku container:login
sudo heroku container:push worker --app questions-bot
sudo heroku container:release worker --app questions-bot