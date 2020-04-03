# Classroom Bot

A bot made for easier management of online classes.
[Invite the bot to your server](https://discordapp.com/api/oauth2/authorize?client_id=691945666896855072&permissions=29486144&scope=bot).
To see a list of commands, use ?help.

## Features

### Draw of hands
Users can raise their hand to answer/ask questions to the teacher using a text command. That way, everyone doesn't speak over eachother in voice chat. The teacher can pick the next student in the queue, pick a random one or pick a specific student. A list of students with their hand up can also be seen.

### Questions
Students can ask questions using a text command. The teacher can then go through them one at a time.

### Your idea here
Got any other ideas? Send me a tweet [@TweetMeepsi](https://twitter.com/TweetMeepsi). Check the [trello](https://trello.com/b/owJzJaVt/classroom-bot) development board to see if i am already working on it first though!

## Building and running
You dont need to run the bot yourself, i am already doing it (see the invite link above). But if you want to run it yourself, here's how to do it. Note: this can only be built on linux. If you want to run it on windows, see the "Running with docker" section.

### Requirements
* CMake
* Make
* A C++ compiler
* libssl-dev
* zlib1g

### Installing aegis.cpp
This bot uses the Aegis library to interface with the Discord API. To install it, follow these steps:
1. Clone the [aegis repository](https://github.com/zeroxs/aegis.cpp)
2. Run `install-deps.sh` (may need sudo)
3. In the aegis repository, run these commands to build it:
```sh
$ mkdir build
$ cd build
$ cmake ..
```
4. Then, to install the library, run: `make install` (may need sudo)

### Setting up classroom bot

1. Start by cloning the repository.
2. Go to your [discord developer applications page](https://discordapp.com/developers/applications) and press "New Application". Give it some name and press the "Copy" button under "client id".
3. In the side bar in your application page, click on "Bot", and press "Add Bot". Give the bot a name, and change the icon if you want.
4. Click the invite link above, but don't add the bot to the server. Instead, replace the numbers after "client_id=" in the URL with your own client id, and press enter. Then you can add your own bot to your server.
5. On the bot page, press the "Copy" button under "token". This token should be kept a secret.
6. In src/main.cpp, replace TOKEN with your token.
7. Then, in the root folder of the repository, run these commands:
```sh
$ mkdir build
$ cd build
$ cmake ..
$ make -j 8
```
8. You can now run the bot by typing `./questionsbot` in the build directory.

## Running with docker
I have created an image on Docker Hub to run the bot easily on linux, mac and windows. If you don't have docker, start by [downlaoding it](https://www.docker.com/). When you have it up and running, the container can be started using this command, which downloads the image and runs it:
```sh
docker run --publish 443:443 --env BOT_TOKEN=YOUR_TOKEN_HERE --detach --name cb meepdocker/classroom-bot:1.0
```
This command may need sudo if you are on linux. The bot can be stopped using:
```sh
docker rm --force cb
```
It can be a good idea to set up the token as an environment variable.

## Developing
If you want to help with development, awesome, here's the basic structure of the project. Please fork the repo and submit a pull request if you have made changes.

### The main class
The main class is questionsbot which lives in main.cpp and main.h. It keeps track of the commands using a map and recieves messages sent in the discord channels on the function onMessage. From here it parses the command and finds the relevant command class to pass it onto.

### Commands
Commands are each represented by a class inheriting from the Command class. These live in src/bot/commands. A command can be registered to the bot via the registerCommand function (see main.cpp for examples). When a command is registered, getCommandInfo is called, which returns a struct containing info about the command (name, aliases). When a command is invoked by a user, the call function is called and passed the remaining parameters and the aegis message object which can be used to respond and otherwise query the Discord API. In the future, this will be replaced with a shared pointer to the aegis::core instance that the command get's passed when it is registered and which should be kept by the class itself.

### Trello
[Trello](https://trello.com/b/owJzJaVt/classroom-bot) is used to keep track of issues (users can submit issues here on github). Mostly just so i can remember what i need to do. If you become a regular contributer, i'll add you to it too, and give you access to the main repo.
