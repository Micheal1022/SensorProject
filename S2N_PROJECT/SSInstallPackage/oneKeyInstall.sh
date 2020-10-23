#!/bin/bash
echo "释放压缩文件"
tar zxvf Qt5.5.1.USER.tar.gz  -C /home/user
tar zxvf SSApp.tar.gz   -C /home/user
tar zxvf libECanVci.so.tar.gz -C /lib
echo "配置系统文件"
cp 20-usb-serial.rules /etc/udev/rules.d/
cp 99-calibration.conf /usr/share/X11/xorg.conf.d
cp profile /etc
echo "安装触屏校准"
dpkg -i xinput-calibrator.deb
echo "软件安装完成"
