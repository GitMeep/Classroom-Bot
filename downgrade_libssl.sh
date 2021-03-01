#!/bin/sh
mkdir libssl
cd libssl
rm libssl-dev_1.1.1-1ubuntu2.1~18.04.7_amd64.deb
rm libssl1.1_1.1.1-1ubuntu2.1~18.04.7_amd64.deb
wget http://launchpadlibrarian.net/523550952/libssl-dev_1.1.1-1ubuntu2.1~18.04.8_amd64.deb
wget http://launchpadlibrarian.net/523550959/libssl1.1_1.1.1-1ubuntu2.1~18.04.8_amd64.deb
sudo dpkg -i libssl-dev_1.1.1-1ubuntu2.1~18.04.8_amd64.deb libssl1.1_1.1.1-1ubuntu2.1~18.04.8_amd64.deb
cd ..