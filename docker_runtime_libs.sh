#!/bin/sh
apt update
apt install -y wget libcurl4 libicu66

mkdir libssl
cd libssl
rm libssl-dev_1.1.1-1ubuntu2.1~18.04.7_amd64.deb
rm libssl1.1_1.1.1-1ubuntu2.1~18.04.7_amd64.deb
wget http://mirrors.kernel.org/ubuntu/pool/main/o/openssl/libssl-dev_1.1.1-1ubuntu2.1~18.04.7_amd64.deb
wget http://mirrors.kernel.org/ubuntu/pool/main/o/openssl/libssl1.1_1.1.1-1ubuntu2.1~18.04.7_amd64.deb
dpkg -i ./libssl-dev_1.1.1-1ubuntu2.1~18.04.7_amd64.deb ./libssl1.1_1.1.1-1ubuntu2.1~18.04.7_amd64.deb
cd ..