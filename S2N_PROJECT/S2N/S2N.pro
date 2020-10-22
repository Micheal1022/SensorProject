#-------------------------------------------------
#
# Project created by QtCreator 2019-11-29T19:57:03
#
#-------------------------------------------------

QT       += core gui sql network serialport multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
#DESTDIR = ./Bevone
DESTDIR = ./Sensor
TARGET = SSApp
TEMPLATE = app

FORMS += \
    MainWindow/mainwindow.ui \
    MainPage/mainpage.ui \
    NodeView/nodeview.ui \
    NodeView/infoview.ui \
    Record/record.ui \
    Login/login.ui \
    SelfCheck/selfcheck.ui \
    MsgBox/infobox.ui \
    MsgBox/questbox.ui \
    Record/datafilter.ui \
    NodeInfo/nodeinfo.ui

HEADERS += \
    MainWindow/mainwindow.h \
    MainPage/mainpage.h \
    CanBusThread/canbusthread.h \
    CanBusThread/ECanVci.h \
    NodeView/nodeview.h \
    NodeView/infoview.h \
    Record/record.h \
    Login/login.h \
    SelfCheck/selfcheck.h \
    MyPrint/myprint.h \
    MsgBox/infobox.h \
    MsgBox/msgbox.h \
    MsgBox/questbox.h \
    CanBusThread/candevice.h \
    SQLite/sqlite.h \
    ControlLight/controllight.h \
    Record/datafilter.h \
    RS485/rs485.h \
    NodeInfo/nodeinfo.h \
    UDPMgm/updmgm.h \
    CanNetUDP/cannetudp.h

SOURCES += \
    MainWindow/main.cpp \
    MainWindow/mainwindow.cpp \
    MainPage/mainpage.cpp \
    CanBusThread/canbusthread.cpp \
    NodeView/nodeview.cpp \
    NodeView/infoview.cpp \
    Record/record.cpp \
    Login/login.cpp \
    SelfCheck/selfcheck.cpp \
    MyPrint/myprint.cpp \
    MsgBox/infobox.cpp \
    MsgBox/msgbox.cpp \
    MsgBox/questbox.cpp \
    CanBusThread/candevice.cpp \
    SQLite/sqlite.cpp \
    ControlLight/controllight.cpp \
    Record/datafilter.cpp \
    RS485/rs485.cpp \
    NodeInfo/nodeinfo.cpp \
    UDPMgm/updmgm.cpp \
    CanNetUDP/cannetudp.cpp

RESOURCES += \
    image.qrc

LIBS +=-L./lib
LIBS +=-lECanVci
LIBS +=-lusb



