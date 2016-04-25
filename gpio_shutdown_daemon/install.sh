#!/bin/sh

# bash install.sh
#  or
# sudo chmod +x install.sh
# ./install.sh
echo ================
echo http://www.neko.ne.jp/~freewing/raspberry_pi/raspberry_pi_3_gpio_switch_shutdown_daemon/
echo ================

# sudo apt-get update
# sudo apt-get -y upgrade
# sudo apt-get -y dist-upgrade

# sudo apt-get -y install git-core

echo ================
echo install libi2c-dev
echo ================
sudo apt-get -y install libi2c-dev

echo ================
echo install wiringPi
echo ================
git clone git://git.drogon.net/wiringPi
cd wiringPi
./build
gpio -v
cd

echo ================
echo install gpio_shutdown_daemon
echo ================
# nano gpio_shutdown_daemon.c
wget https://raw.githubusercontent.com/FREEWING-JP/RaspberryPi_Files/master/gpio_shutdown_daemon/gpio_shutdown_daemon.c
gcc -o gpio_shutdown_daemon gpio_shutdown_daemon.c -I/usr/local/include -L/usr/local/lib -lwiringPi

echo ================
echo install supervisor
echo ================
sudo apt-get -y install supervisor
# sudo nano /etc/supervisor/conf.d/gpio_shutdown_daemon.conf
wget https://raw.githubusercontent.com/FREEWING-JP/RaspberryPi_Files/master/gpio_shutdown_daemon/gpio_shutdown_daemon.conf
sudo cp gpio_shutdown_daemon.conf /etc/supervisor/conf.d/gpio_shutdown_daemon.conf
sudo supervisorctl reread
sudo supervisorctl add gpio_shutdown_daemon
sudo supervisorctl status

echo ================
echo Done
echo ================
