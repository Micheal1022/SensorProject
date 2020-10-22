#include "myprint.h"
#include <QSerialPort>
#include <QDebug>

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
    //"/dev/ttymxc1"

    myCom = new QSerialPort(printName);
    if(myCom->isOpen())
    {
        myCom->close();
    }

    if(myCom->open(QIODevice::ReadWrite))
    {
        myCom->setBaudRate(QSerialPort::Baud9600);
        //数据位设置，我们设置为8位数据位
        myCom->setDataBits(QSerialPort::Data8);
        //奇偶校验设置，我们设置为无校验
        myCom->setParity(QSerialPort::NoParity);
        //停止位设置，我们设置为1位停止位
        myCom->setStopBits(QSerialPort::OneStop);
        //数据流控制设置，我们设置为无数据流控制
        myCom->setFlowControl(QSerialPort::NoFlowControl);
        qDebug()<<"Open print OK !";
    }
    else
    {
        qDebug()<<"Open print FAIL !";
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

void MyPrint::selfCheck()
{
    QString compelet =  tr("自检完成");
    QByteArray byte = compelet.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
}

void MyPrint::dataPring(QString pass, QString canId, QString state, QString time, QString addr)
{
    QByteArray byte;
    //打印地址
    QString modAddr   = tr("区域 : ") + addr;
    byte = modAddr.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //打印时间
    QString modTime   = tr("时间 : ") + time;
    byte = modTime.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //数值
    //    QString modValue  = tr("数值 : ") + value;
    //    byte = modValue.toLocal8Bit();
    //    myCom->write(byte);
    //    myCom->write(lf,1);
    //状态
    QString modStatus = tr("状态 : ") + state;
    byte = modStatus.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //打印网络和地址
    QString modNetId  = tr("通道 : ")+ pass +tr("    节点 : ")+ canId;
    byte = modNetId.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
}

void MyPrint::autoPring(int pass, int canId, int state, QString time, QString addr)
{
    QByteArray byte;
    //打印地址
    QString modAddr   = tr("区域 : ") + addr;
    byte = modAddr.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //打印时间
    QString modTime   = tr("时间 : ") + time;
    byte = modTime.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //状态
    QString modSts;
    switch (state) {
    case NodeOverCurrent:
        modSts = tr("节点过流");
        break;
    case NodeLackPhase:
        modSts = tr("节点错相");
        break;
    case NodeOverVoltage:
        modSts = tr("节点过压");
        break;
    case NodeUnderVoltage:
        modSts = tr("节点欠压");
        break;
    case NodePowerLost:
        modSts = tr("供电中断");
        break;
    case NodeCanBusError:
        modSts = tr("通讯故障");
        break;
    case MainPowerError:
        modSts = tr("主电故障");
        break;
    case BackPowerError:
        modSts = tr("备电故障");
        break;
    }

    QString modStatus = tr("状态 : ") + modSts;
    byte = modStatus.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
    //打印网络和地址
    QString modNetId  = tr("通道 : ")+ QString::number(pass) +tr("    节点 : ")+ QString::number(canId);
    byte = modNetId.toLocal8Bit();
    myCom->write(byte);
    myCom->write(lf,1);
}


