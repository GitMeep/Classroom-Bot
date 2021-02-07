[![Discord Bots](https://top.gg/api/widget/status/691945666896855072.svg)](https://top.gg/bot/691945666896855072) [![Discord Bots](https://top.gg/api/widget/servers/691945666896855072.svg)](https://top.gg/bot/691945666896855072)

# Classroom Bot

A bot made for helping with online teaching on Discord.
[Invite the bot to your server](https://discordapp.com/api/oauth2/authorize?client_id=691945666896855072&permissions=297888850&scope=bot).
To see a list of commands, use `?help`.

## Features

### Draw of hands
Students can raise their hand to indicate an answer to a question or ask one. The teacher can pick the next student in the queue, pick a random one or pick a specific student. That way, everyone doesn't speak at the same time in voice chat.

### Questions
Students can ask questions directly using a text command. The teacher can then go through them one at a time. Useful when students are working in groups and not in the same voice channel as the teacher.

### Mute
Mute everyone in a voice channel except teachers. Useful for when students won't be quiet. Discord does also have a priority speaker feature which might be used instead.

### PubChem lookup
Look up safety information and chemical properties of a compound on PubChem.

### Multilingual
Classroom Bot supports multiple languages, currently
 - English
 - Dutch
 - Danish

Don't see your language? You can help translating by [contacting me](https://discord.gg/dqmTAZY).
### Your idea here
Got any other ideas? Tell me via the [support server](https://discord.gg/dqmTAZY). Check the [trello](https://trello.com/b/owJzJaVt/classroom-bot) development board to see if i am already working on it first though.

### Supporting
If you find the bot useful, please concider supporting me on [Github Sponsors](https://github.com/sponsors/GitMeep)

## Getting started
To get started using the bot, [invite it to your server](https://discordapp.com/api/oauth2/authorize?client_id=691945666896855072&permissions=297888850&scope=bot) and use the command `?help` to see a list of commands. Use `?help [command]` to get detailed info about a command.

# Building and running
You dont need to run the bot yourself, i am already doing it (see the invite link above). But if you want to run it yourself, here's how to do it. Note: this can only be built on linux. If you want to run it on windows, see the "Running with docker" section.

### Building and Configuring
Note: it is easier to just run the bot as a docker image, see [Running with docker](#Running-with-docker). This procedure is made for ubuntu 20.04, it might not work on other distros.


If you are on windows, i suggest using an [ubuntu container to develop in visual studio code](https://code.visualstudio.com/docs/remote/containers). The configuration for this repository already exists in the `.devcontainer` folder. Just press the little icon in the lower left corner of the VSCode window while the docker daemon is running and select `Remote-Container: Open Folder in Container`, then open the repository folder.

Prerequisites:
You need a Mongo database to run the bot. I suggest using [MongoDB Atlas](https://www.mongodb.com/cloud/atlas) to run one in the cloud. You can also run it locally.

1. After cloning this repo, run the setup script. This will install all the dependencies. It might take a while:

```
$ ./setup.sh
```

2. then you can build the bot
```sh
$ make -j4 config=release
$ cp ./default_config.json ./bin/config.json
```

3. The bot can now be started by running `./questionsbot` in the `bin` directory. To configure the bot (set the token), edit bin/config.json. Note: If you are developing in a container, port 443 needs to be forwarded.

The default config file looks like this:
```json
{
    "bot": {
        "token": "${BOT_TOKEN}",
        "owner": "${OWNER_ID}"
    },
    "persistence": {
        "url": "${MONGO_URL}",
        "db_name": "${DATABASE_NAME}",
        "encryption_key": "${ENCRYPTION_KEY}"
    },
    "topgg": {
        "enable": "${ENABLE_TOPGG}",
        "bot_id": "${TOPGG_ID}",
        "token": "${TOPGG_TOKEN}"
    }
}
```
`url` is your MongoDB URL, in the form `mongodb+src://username:password@host:port`. The rest should be pretty obvious.

top.gg should stay disabled for third parties running the bot.
As you can see, the default values are environment variables. When you specify a value in the format `${NAME}` the bot will try replacing it with the environment variable of the same name. If no variable is found, the field is left blank.
Fields with `enable` or `disable` in the name are booleans.

## Encryption
To generate the key, use this command:
```sh
openssl enc -aes-128-cfb -P -pbkdf2
```
Enter some random password, you will only need to enter it twice, then you can forget it.
Copy the key and into an environment variable or directly into the config file. There might be a better, safer way to store these values, if you know a way that works with heroku, please contact me.

## Running with docker
I have created an image on Docker Hub to run the bot easily on Linux, Mac or Windows. If you don't have Docker, start by [downlaoding it](https://www.docker.com/).
When you have Docker installed, you can run the container via docker-compose or a manual command.
### docker-compose
If you are on Mac or Windows, i recommend just running the container with Docker Desktop where you can easily download the image and set environment variables. If you want to run with docker-compose, i have created a `docker-compose.yml` file in this repository. Just download that file alone, put it in a folder and fill in the required fields. Then to start it, run
```sh
docker-compose up -d
```
To see the logs, run
```sh
docker-compose logs -f
```
To stop the bot again use
```sh
docker-compose down
```
### docker
You can also just run the container with a single Docker command:
```sh
docker run --publish 443:443 --env BOT_TOKEN=YOUR_TOKEN_HERE --env DATABASE_URL=YOUR_DB_URL_HERE --env ENCRYPTION_KEY=YOUR_ENCRYPTION_KEY_HERE --detach --name cb meepdocker/classroom-bot:latest
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

# License
See the [LICENSE](LICENSE.txt) file
