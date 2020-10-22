#!/bin/bash

tar zxvf Qt5.5.1.USER.tar.gz  -C /home/user
tar zxvf SSApp.tar.gz   -C /home/user
tar zxvf libECanVci.so.tar.gz -C /lib

cp 20-usb-serial.rules /etc/udev/rules.d/
cp profile /etc
echo "安装完成,请在删除任务栏后关机"
