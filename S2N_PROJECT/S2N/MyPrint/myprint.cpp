#include "myprint.h"
#include <QSerialPort>
#include <QDebug>
#include <QDateTime>
char MyPrint::lf[2]={0x0A,'\0'};
MyPrint *MyPrint::print = NULL;
QSerialPort *MyPrint::myCom = NULL;

MyPrint::MyPrint()
{

}

MyPrint::~MyPrint()
{
    delete print;
}

void MyPrint::initSerialPort(QString printName)
{
    myCom = new QSerialPort(printName);
    myCom->setBaudRate(QSerialPort::Baud9600);
    //数据位设置，我们设置为8位数据位
    myCom->setDataBits(QSerialPort::Data8);
    //奇偶校验设置，我们设置为无校验
    myCom->setParity(QSerialPort::NoParity);
    //停止位设置，我们设置为1位停止位
    myCom->setStopBits(QSerialPort::OneStop);
    //数据流控制设置，我们设置为无数据流控制
    myCom->setFlowControl(QSerialPort::NoFlowControl);
    if (myCom->open(QIODevice::ReadWrite)) {
        qDebug()<<"OPEN ttyS1 OK!";
    } else {
        qDebug()<<"OPEN ttyS1 ERROR!";
    }
}

MyPrint *MyPrint::getInstance()
{
    if(print == NULL)
    {
        print = new MyPrint();
    }
    return print;
}

void MyPrint::selfCheckFinished()
{
    //打印时间
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-mm-dd hh:MM:ss");
    QString modTime  = tr("时间 : ") + currentTime;
    QByteArray byte = modTime.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    QString compelet =  tr("************自检完成**********");
    byte = compelet.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
}

void MyPrint::dataPring(QString pass, QString canId, QString type,QString state, QString value,QString time, QString addr)
{
    QByteArray byte;
    //打印地址
    QString modAddr  = tr("区域 : ") + addr;
    byte = modAddr.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //打印时间
    QString modTime  = tr("时间 : ") + time;
    byte = modTime.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //状态+数值
    QString modState=  tr("状态 : ") + state;
    byte = modState.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //状态
    QString modType =  tr("类型 : ") + type +tr("     数值 : ") + value;;
    byte = modType.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //打印网络和地址
    QString modNetId = tr("通道 : ")+ pass + tr("        节点 : ")+ canId;
    byte = modNetId.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
}



