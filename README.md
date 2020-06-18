[![Discord Bots](https://top.gg/api/widget/status/691945666896855072.svg)](https://top.gg/bot/691945666896855072)

# Classroom Bot

A bot made for easier management of online classes.
[Invite the bot to your server](https://discordapp.com/api/oauth2/authorize?client_id=691945666896855072&permissions=297888850&scope=bot).
To see a list of commands, use ?help.

## Features

### Draw of hands
Users can raise their hand to answer/ask questions to the teacher using a text command. That way, everyone doesn't speak over eachother in voice chat. The teacher can pick the next student in the queue, pick a random one or pick a specific student. A list of students with their hand up can also be seen.

### Questions
Students can ask questions using a text command. The teacher can then go through them one at a time.

### Mute
Tired of students talking when they shouldn't? Force mute a channel, so only the teacher can speak.

### PubChem lookup
Look up safety information and molar mass of a compound on PubChem.

### Your idea here
Got any other ideas? Send me a tweet [@TweetMeepsi](https://twitter.com/TweetMeepsi). Check the [trello](https://trello.com/b/owJzJaVt/classroom-bot) development board to see if i am already working on it first though!

## Getting started
To get started using the bot, invite it to your server and assign yourself a role named "Teacher". This makes it so you can use the admin-only commands.

## Building and running
You dont need to run the bot yourself, i am already doing it (see the invite link above). But if you want to run it yourself, here's how to do it. Note: this can only be built on linux. If you want to run it on windows, see the "Running with docker" section.

### Building and Configuring
#### Note: it is easier to just run the bot as a docker image, see [Running with docker](#Running-with-docker)
1. Start by installing the dependencies:
`sudo apt-get install -y libssl-dev zlib1g libpq-dev libcurl4-openssl-dev git libcurl4 automake libtool make g++ g++-9`

2. Then, clone my fork of aegis:
`$ git clone https://github.com/GitMeep/aegis.cpp.git`

3. Build and install aegis:
```sh
$ cd aegis.cpp
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
$ cd ../..
```

4. Then clone and install restclient-cpp
```sh
$ git clone https://github.com/mrtazz/restclient-cpp.git
$ cd restclient-cpp
$ ./autogen.sh
$ ./configure
$ sudo make install
$ cd ..
```

5. lastly, clone and build the bot
```sh
$ git clone https://github.com/GitMeep/Classroom-Bot.git
$ cd classroombot
$ premake5 gmake
$ make -j8 config=release
$ cp ./default_config.json ./bin/config.json
```

6. The bot can now be started by running `./questionsbot` in the `bin` directory. To configure the bot (set the token), edit ./bin/config.json

The default config file looks like this:
```json
{
    "bot": {
        "token": "${BOT_TOKEN}"
    },
    "persistence": {
        "enable": "${ENABLE_PERSISTENCE}",
        "url": "${DATABASE_URL}"
    },
    "topgg": {
        "enable": "${ENABLE_TOPGG}",
        "bot_id": "${TOPGG_ID}",
        "token": "${TOPGG_TOKEN}"
    }
}
```
`bot` is where you put information about the discord bot.
`persitence` is your postgressql information. URL is in the form `postgres://username:password@host:port/database`.
`topgg` is for updating server count on top.gg third parties running the bot shouldn't post it on top.gg, so this should be disabled.
As you can see, the default values are environment variables. When you specify a value in the format `${NAME}` the bot will try replacing it with the environment variable of the same name. If no variable is found, the field is left blank.

## Running with docker
I have created an image on Docker Hub to run the bot easily on linux, mac and windows. If you don't have docker, start by [downlaoding it](https://www.docker.com/). When you have it up and running, the container can be started using this command, which downloads the image and runs it:
```sh
docker run --publish 443:443 --env BOT_TOKEN=YOUR_TOKEN_HERE --detach --name cb meepdocker/classroom-bot:latest
```
This command may need sudo if you are on linux. The bot can be stopped using:
```sh
docker rm --force cb
```
Again, it can be a good idea to set up the token as an environment variable.

## Developing
If you want to help with development, awesome! Please fork the repo and submit a pull request if you want to make changes. If you want an introduction to the code, please message me on the [support server](https://discord.gg/dqmTAZY).

### Trello
[Trello](https://trello.com/b/owJzJaVt/classroom-bot) is used to keep track of tasks. Mostly just so i can remember what i need to do. If you become a regular contributer, i'll add you to it too, and give you access to the main repo.
