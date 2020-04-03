
cd build
make -j 10
cd ..

sudo heroku container:push worker --app questions-bot
sudo heroku container:release worker --app questions-bot