#!/bin/sh

mkdir dpp
cd dpp
wget https://github.com/brainboxdotcc/DPP/releases/download/v9.0.13/libdpp-9.0.13-linux-x64.deb
apt install -y ./libdpp-9.0.13-linux-x64.deb