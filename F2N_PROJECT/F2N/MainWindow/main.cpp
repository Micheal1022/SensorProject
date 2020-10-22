#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QMetaType>
#include <QUdpSocket>
#include "CoreControl/corecontrol.h"
int main(int argc, char *argv[])
{
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForLocale(codec);
    QApplication a(argc, argv);

    qRegisterMetaType<QList<qreal> >("QList<qreal>");
    CoreControl coreControl;
    coreControl.initObj();



//    QUdpSocket qus;
//    QByteArray msg = "Hello world!";
//    for(int i=0; i<100; ++i)
//        qus.writeDatagram(msg, QHostAddress("192.168.126.62"), 8001);
    return a.exec();
}
