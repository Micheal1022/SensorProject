#include "controllight.h"
#include <QDebug>
#include <QByteArray>
#include <QSerialPort>
#include <QTimer>
#include <QSound>
#include <QSettings>
#include <QCoreApplication>
#define RECVTIME    1000
#define SENDTIME    500
#define DATASIZE    5

#define CMD_HEAD    0
#define CMD_CODE    1
#define CMD_DATA_1  2
#define CMD_DATA_2  3
#define CMD_TAIL    4

#define ALARMSOUND  1
#define ERRORSOUND  2
#define NORMALSOUND 3

uchar cmd_Head = 0x55;
uchar cmd_Code = 0x03;
uchar cmd_Tail = 0xEE;

uchar cmd_Power= 0x22;
uchar cmd_reset= 0x33;

uchar cmd_sw_on = 1;
uchar cmd_sw_off= 0;

uchar cmd_MPNormal = 0x01;
uchar cmd_BPNormal = 0x02;
uchar cmd_MPError  = 0x03;
uchar cmd_BPError  = 0x04;
uchar cmd_alarmLed = 0x05;
uchar cmd_errorLed = 0x06;
uchar cmd_canLed   = 0x07;
uchar cmd_muteLed  = 0x08;
uchar cmd_relayCtrl= 0x09;
uchar cmd_allCtl   = 0xFF;
uchar cmd_power    = 0xFF;


ControlLight::ControlLight(QObject *parent) : QObject(parent)
{

    initSerialDevice();
    initVariable();
    initConnect();
}

ControlLight::~ControlLight()
{
    closeSerialDevice();
    delete m_errorSound;
    delete m_alarmSound;
    m_dataTimer->stop();
    delete m_dataTimer;
    delete m_serialPort;
}

void ControlLight::initVariable()
{
    m_step = 0;
    m_mainPower = 1;
    m_backPower = 1;
    m_powerState = confSendData(cmd_power,cmd_sw_off);
    m_allCtrl = confSendData(cmd_sw_off,cmd_allCtl);
    sendDataSerial(m_allCtrl);

    m_errorSound = new QSound(":/Sound/error.wav");
    m_errorSound->setLoops(QSound::Infinite);
    m_errorSound->stop();

    m_alarmSound = new QSound(":/Sound/alarm.wav");
    m_alarmSound->setLoops(QSound::Infinite);
    m_alarmSound->stop();

}


void ControlLight::initConnect()
{
    connect(m_serialPort,SIGNAL(readyRead()),this,SLOT(slotRecvData()));

    m_dataTimer = new QTimer;
    connect(m_dataTimer,SIGNAL(timeout()),this,SLOT(slotDataTimeOut()),Qt::DirectConnection);
    m_dataTimer->start(SENDTIME);

}

void ControlLight::controlReset()
{
    m_allCtrl = confSendData(cmd_sw_off,cmd_allCtl);
    sendDataSerial(m_allCtrl);
}

void ControlLight::initSerialDevice()
{
    if (true == openSerialDevice()) {
        qDebug()<<"OPEN ttyS0 OK!";
    } else {
        qDebug()<<"OPEN ttyS0 ERROR!";
    }
}

bool ControlLight::openSerialDevice()
{
    QSettings settings(QCoreApplication::applicationDirPath()+"/Conf.ini", QSettings::IniFormat);
    QString lightName = settings.value("LIGHT/LightName").toString();
    m_serialPort = new QSerialPort(lightName);
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
    //设置缓存大小
    //m_serialPort->setReadBufferSize(DATASIZE);
    if (m_serialPort->open(QIODevice::ReadWrite)) {
        return true;
    }
    return false;
}

void ControlLight::closeSerialDevice()
{
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }
}

QByteArray ControlLight::confSendData(uchar data_1, uchar data_2)
{
    QByteArray byteArray;
    byteArray.append(cmd_Head);
    byteArray.append(cmd_Code);
    byteArray.append(data_1);
    byteArray.append(data_2);
    byteArray.append(cmd_Tail);
    return byteArray;
}

void ControlLight::slotConfSerialData(QList<int> dataList)
{
    //主点指示灯，备电指示灯，主电故障指示灯，备电故障指示灯，故障指示灯，报警指示灯，通讯指示灯，消音指示灯
    uchar ErrorLed = dataList.at(0);
    uchar AlarmLed = dataList.at(1);
    uchar MuteLed  = dataList.at(2);

    if (m_mainPower > 0) {
        m_MPNormal = confSendData(cmd_sw_on,cmd_MPNormal);
        m_MPError  = confSendData(cmd_sw_off,cmd_MPError);
    } else {
        m_MPNormal = confSendData(cmd_sw_off,cmd_MPNormal);
        m_MPError  = confSendData(cmd_sw_on,cmd_MPError);
    }

    if (m_backPower > 0) {
        m_BPError  = confSendData(cmd_sw_off,cmd_BPError);
        m_BPNormal = confSendData(cmd_sw_on,cmd_BPNormal);
    } else {
        m_BPError  = confSendData(cmd_sw_on,cmd_BPError);
        m_BPNormal = confSendData(cmd_sw_off,cmd_BPNormal);
    }

    if (ErrorLed > 0) {
        m_errorLed = confSendData(cmd_sw_on,cmd_errorLed);
    } else {
        m_errorLed = confSendData(cmd_sw_off,cmd_errorLed);
    }

    if (AlarmLed > 0) {
        m_alarmLed = confSendData(cmd_sw_on,cmd_alarmLed);
        m_relayCtrl= confSendData(cmd_sw_on,cmd_relayCtrl);
    } else {
        m_alarmLed = confSendData(cmd_sw_off,cmd_alarmLed);
        m_relayCtrl= confSendData(cmd_sw_off,cmd_relayCtrl);
    }

    if (MuteLed > 0) {
        m_muteLed = confSendData(cmd_sw_on,cmd_muteLed);
    } else {
        m_muteLed = confSendData(cmd_sw_off,cmd_muteLed);
    }
}

void ControlLight::sendDataSerial(QByteArray byteArray)
{
//    qDebug("*******************************");
//    qDebug()<<"byteArray[HEAD] = "<<(uchar)byteArray[0];
//    qDebug()<<"byteArray[CMD ] = "<<(uchar)byteArray[1];
//    qDebug()<<"byteArray[DATA] = "<<(uchar)byteArray[2];
//    qDebug()<<"byteArray[DATA] = "<<(uchar)byteArray[3];
//    qDebug()<<"byteArray[TAIL] = "<<(uchar)byteArray[4];
    if (5 == byteArray.size()) {
        m_serialPort->write(byteArray);
    }
}

void ControlLight::slotDataTimeOut()
{
    m_step++;
    if (9 == m_step) {
        m_step = 0;
    }
    switch (m_step) {
    case 0:
        sendDataSerial(m_powerState);
        break;
    case 1:
        sendDataSerial(m_MPNormal);
        break;
    case 2:
        sendDataSerial(m_MPError);
        break;
    case 3:
        sendDataSerial(m_BPNormal);
        break;
    case 4:
        sendDataSerial(m_BPError);
        break;
    case 5:
        sendDataSerial(m_powerState);
        break;
    case 6:
        sendDataSerial(m_errorLed);
        break;
    case 7:
        sendDataSerial(m_alarmLed);
        sendDataSerial(m_relayCtrl);
        break;
    case 8:
        sendDataSerial(m_muteLed);
        break;
    }
}

void ControlLight::slotRecvData()
{
    QByteArray byteArray = m_serialPort->read(DATASIZE);
    m_recvByteArray += byteArray;
    if (m_recvByteArray.size() == DATASIZE && m_recvByteArray.at(CMD_HEAD)== cmd_Head) {
//        qDebug("*******************************");
//        qDebug()<<"byteArray[HEAD] = "<<(uchar)m_recvByteArray[0];
//        qDebug()<<"byteArray[CMD ] = "<<(uchar)m_recvByteArray[1];
//        qDebug()<<"byteArray[DATA] = "<<(uchar)m_recvByteArray[2];
//        qDebug()<<"byteArray[DATA] = "<<(uchar)m_recvByteArray[3];
//        qDebug()<<"byteArray[TAIL] = "<<(uchar)m_recvByteArray[4];
        if (m_recvByteArray.at(CMD_CODE) == cmd_Power) {
            m_mainPower = m_recvByteArray.at(CMD_DATA_1);
            m_backPower = m_recvByteArray.at(CMD_DATA_2);
            emit sigPowerState(m_mainPower,m_backPower);
        }
        else if (m_recvByteArray.at(CMD_CODE) == cmd_reset) {
            emit sigReset();
        }
        m_recvByteArray.clear();
    }

    if (m_recvByteArray.size() > DATASIZE) {
        m_recvByteArray.clear();
    }
}


void ControlLight::slotSendDataSerial(uchar data_1, uchar data_2)
{
    QByteArray byteArray = confSendData(data_1,data_2);
    sendDataSerial(byteArray);
}

void ControlLight::setAlarmSound()
{
    m_errorSound->stop();
    m_alarmSound->play();
}

void ControlLight::setErrorSound()
{
    m_errorSound->play();
    m_alarmSound->stop();
}

void ControlLight::setNormalSound()
{
    m_alarmSound->stop();
    m_errorSound->stop();
}

bool ControlLight::alarmIsFinished()
{
    return m_alarmSound->isFinished();
}

bool ControlLight::errorIsFinished()
{
    return m_errorSound->isFinished();
}

void ControlLight::controlCanLed(bool flag)
{
    if (true == flag) {
        m_canLed = confSendData(cmd_sw_on,cmd_canLed);
        sendDataSerial(m_canLed);
    } else {
        m_canLed = confSendData(cmd_sw_off,cmd_canLed);
        sendDataSerial(m_canLed);
    }
}

void ControlLight::controlDataTimer(bool flag)
{
    if (true == flag) {
        m_dataTimer->start(SENDTIME);
    } else {
        m_dataTimer->stop();
    }

}

void ControlLight::slotControlSound(int soundType)
{
    switch (soundType) {
    case NORMALSOUND:
        setNormalSound();
        break;
    case ALARMSOUND:
        setAlarmSound();
        break;
    case ERRORSOUND:
        setErrorSound();
        break;
    }
}


