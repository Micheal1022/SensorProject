#include "iomanager.h"
#include <QDebug>
#include <QSerialPort>
#include <QtMultimedia/QSound>

const char head = 0x55;//起始帧
const char tail = 0xEE;
const char data = 0x00;
const char light= 0xFF;

//write power
const char cmdPower = 0x03;
const char cmdDate  = 0xFF;
//write light
const char cmdLight = 0x05;
//read power,reset
const char PowerSts = 0x22;
const char CmdReset = 0x33;




IOManager::IOManager(QObject *parent) : QObject(parent)
{

}

IOManager::IOManager(QString IOName)
{
    initSerialPort(IOName);
    errorSound = new QSound(":/sound/error.wav");
    errorSound->setLoops(QSound::Infinite);
    errorSound->stop();
}

IOManager::~IOManager()
{
    delete errorSound;
    m_serialPort->close();
    delete m_serialPort;
}

void IOManager::initSerialPort(QString IOName)
{
    m_serialPort = new QSerialPort;
    if(m_serialPort->isOpen())
        m_serialPort->close();
    //设置串口名
    m_serialPort->setPortName(IOName);
    //设置波特率
    m_serialPort->setBaudRate(QSerialPort::Baud9600);
    //设置数据位数
    m_serialPort->setDataBits(QSerialPort::Data8);
    //设置停止位
    m_serialPort->setStopBits(QSerialPort::OneStop);
    //设置奇偶校验
    m_serialPort->setParity(QSerialPort::NoParity);
    //设置流控制
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if(m_serialPort->open(QIODevice::ReadWrite))
    {
        qDebug()<<IOName<<"SerialPort OK";
        //m_serialPort->setReadBufferSize(5);
    }
    else
    {
        qDebug()<<IOName<<"SerialPort ERROR !";
    }
    connect(m_serialPort,&QSerialPort::readyRead,this,&IOManager::slotReadyRead);
}


void IOManager::slotReadyRead()
{
    QByteArray byteArray = m_serialPort->readAll();
    m_byteArray += byteArray;

    if (byteArray.size() == 5) {
        int cmd = byteArray.at(1);
        switch (cmd) {
        case PowerSts:
        {
            int mainPower = byteArray.at(2);
            int backPower = byteArray.at(3);
            emit sigPowerState(mainPower,backPower);
        }
            break;
        case CmdReset:
            emit sigResetSwitch();
            break;
        }
        m_byteArray.clear();
    }

    if (m_byteArray.size() > 5) {
        m_byteArray.clear();
    }
}

void IOManager::slotControlLight(char type, char control)
{
    QByteArray byteArray;
    byteArray.append(head);
    byteArray.append(cmdLight);
    byteArray.append(control);
    byteArray.append(type);
    byteArray.append(tail);
    //qDebug()<<"byteArray = "<<byteArray;
    m_serialPort->write(byteArray);
}

void IOManager::slotControlSound(int type)
{
    switch (type)
    {
    case 0:
        errorSound->stop();
        break;
    case 1:
        errorSound->play();
        break;
    }
}
