#-------------------------------------------------
#
# Project created by QtCreator 2018-11-22T14:45:43
#
#-------------------------------------------------

QT       += core gui sql network serialport multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR = ./SSApp
TARGET = SSApp
TEMPLATE = app

SOURCES += \
    MainWindow/mainwindow.cpp \
    MainWindow/main.cpp \
    NodeInfo/nodeinfo.cpp \
    GlobalData/globaldata.cpp \
    CoreControl/corecontrol.cpp \
    CanBusThread/canbusthread.cpp \
    UserLogin/userlogin.cpp \
    SQLite/sqlite.cpp \
    NodeManager/nodemanager.cpp \
    RecordQuery/recordquery.cpp \
    RecordQuery/datafilter.cpp \
    NodeManager/nodetablewgt.cpp \
    MainPage/mainpage.cpp \
    MsgBox/infobox.cpp \
    MsgBox/msgbox.cpp \
    MsgBox/questbox.cpp \
    MyPrint/myprint.cpp \
    IOManager/iomanager.cpp \
    NodeInfo/stateinfo.cpp \
    SelfCheck/selfcheck.cpp \
    UDPServer/udpserver.cpp \
    RS485/rs485.cpp \
    CanNetUDP/cannetudp.cpp

HEADERS += \
    MainWindow/mainwindow.h \
    NodeInfo/nodeinfo.h \
    GlobalData/globaldata.h \
    CoreControl/corecontrol.h \
    CanBusThread/ECanVci.h \
    CanBusThread/canbusthread.h \
    UserLogin/userlogin.h \
    SQLite/sqlite.h \
    NodeManager/nodemanager.h \
    RecordQuery/recordquery.h \
    RecordQuery/datafilter.h \
    NodeManager/nodetablewgt.h \
    MainPage/mainpage.h \
    MsgBox/infobox.h \
    MsgBox/msgbox.h \
    MsgBox/questbox.h \
    MyPrint/myprint.h \
    IOManager/iomanager.h \
    NodeInfo/stateinfo.h \
    SelfCheck/selfcheck.h \
    UDPServer/udpserver.h \
    RS485/rs485.h \
    CanNetUDP/cannetudp.h

FORMS += \
    MainWindow/mainwindow.ui \
    NodeInfo/nodeinfo.ui \
    UserLogin/userlogin.ui \
    NodeManager/nodemanager.ui \
    RecordQuery/recordquery.ui \
    RecordQuery/datafilter.ui \
    NodeManager/nodetablewdt.ui \
    MainPage/mainpage.ui \
    MsgBox/infobox.ui \
    MsgBox/questbox.ui \
    NodeInfo/stateinfo.ui \
    SelfCheck/selfcheck.ui

RESOURCES += \
    image.qrc


LIBS +=-L./lib
LIBS +=-lECanVci
LIBS +=-lusb
