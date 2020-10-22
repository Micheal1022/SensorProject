#include "rs485.h"
#include <QDebug>
#include <QSettings>
#include <QDateTime>
#include <QTimeZone>
#include <time.h>
#include <QSerialPort>
#include <QCoreApplication>
#include <QTimer>
#include <QProcess>
#include "GlobalData/globaldata.h"
#define BUFFSIZE    15
#define CRC_L_TIME  13
#define CRC_H_TIME  14
#define CRC_L   6
#define CRC_H   7
#define STARTADDR  0x2710
#define PASS_2_ADD 256

#define PASS_1  1
#define PASS_2  2

#define YEAR    7
#define MONTH   8
#define DAY     9
#define HOUR    10
#define MINUTE  11
#define SECOND  12

char null_d = 0x00;
char host_add = 0x01;
char code_func = 0x06;
char code_power = 0x03;
quint16 reg_mute  = 0x2333;//消音
quint16 reg_back  = 0x2335;//备电
quint16 reg_power = 0x2334;//主电
quint16 reg_reset = 0x2332;//复位


uchar CRC_High[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
uchar CRC_Low[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

RS485::RS485(QObject *parent) : QObject(parent)
{
    m_mainPower = 0;
    m_backPower = 0;
    m_hostNumber= 1;
    initProt();
    initConnect();
}

RS485::~RS485()
{
    delete m_serialPort;
}

void RS485::initProt()
{
    //初始化状态为0
    for (int i = 0; i < 2040;  i++) {
        m_nodeArray[i].state = 0;
    }
    //    QSettings settings(QCoreApplication::applicationDirPath()+"./Conf.ini", QSettings::IniFormat);
    QSettings settings("./Conf.ini", QSettings::IniFormat);
    m_hostNumber = settings.value("RS485/HostNumber").toInt();
    int baudRate = settings.value("RS485/BaudRate").toInt();
    int dataBits = settings.value("RS485/DataBits").toInt();
    int stopBits = settings.value("RS485/StopBits").toInt();
    int parity   = settings.value("RS485/Parity").toInt();
    QString RS485Port = settings.value("RS485/RS485Port").toString();

//    qDebug()<<"*******************";
//    qDebug()<<"m_hostNumber : "<<m_hostNumber;
//    qDebug()<<"baudRate     : "<<baudRate;
//    qDebug()<<"dataBits     : "<<dataBits;
//    qDebug()<<"stopBits     : "<<stopBits;
//    qDebug()<<"parity       : "<<parity;
//    qDebug()<<"RS485Port    : "<<RS485Port;

    m_serialPort = new QSerialPort;
    if(m_serialPort->isOpen())
        m_serialPort->close();
    //设置串口名
    m_serialPort->setPortName(RS485Port);
    //设置波特率
    m_serialPort->setBaudRate((QSerialPort::BaudRate)baudRate);
    //设置数据位数
    m_serialPort->setDataBits((QSerialPort::DataBits)dataBits);
    //设置停止位
    m_serialPort->setStopBits(QSerialPort::StopBits(stopBits));
    //设置奇偶校验
    m_serialPort->setParity(QSerialPort::Parity(parity));
    //设置流控制
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (m_serialPort->open(QIODevice::ReadWrite)) {
        qDebug()<<"OPEN ttyS5 OK!";
        //m_serialPort->setReadBufferSize(1);
    } else {
        qDebug()<<"OPEN ttyS5 ERROR!";
    }





}

void RS485::initConnect()
{
    m_timer = new QTimer;
    connect(m_timer,SIGNAL(timeout()),this,SLOT(slotTimeOut()));
    m_timer->start(200);
    //connect(m_serialPort,&QSerialPort::readyRead,this,&RS485::slotRecvAll);
}

quint16 RS485::dataCRC16(uchar *data, int len)
{
    uchar uchCRCHi = 0xff;
    uchar uchCRCLo = 0xff;
    quint16  uindex;
    while(len--) {
        uindex  = uchCRCLo ^ *data++;
        uchCRCLo = uchCRCHi ^ CRC_High[uindex];
        uchCRCHi = CRC_Low[uindex];
    }
    return (uchCRCLo << 8 | uchCRCHi);
}

void RS485::sendData(char code, qint16 reg, qint16 date)
{
    QByteArray ba;
    ba.append(m_hostNumber);
    ba.append(code);
    ba.append(reg>>8);
    ba.append(reg&0xFF);
    ba.append(date>>8);
    ba.append(date&0xFF);
    char *data = ba.data();
    qint16 crcNum = dataCRC16((uchar*)data,ba.size());

    ba.append(crcNum >> 8);
    ba.append(crcNum & 0xFF);

    m_serialPort->write(ba);
}

void RS485::sendData(char code, char *data, int len)
{
    QByteArray ba;
    ba.append(m_hostNumber);
    ba.append(code);
    ba.append(len);
    for (int i = 0;i < len;i++) {
        ba.append((uchar)data[i]);
    }

    char *data_crc = ba.data();
    quint16 crcNum = dataCRC16((uchar*)data_crc,ba.size());

    ba.append(crcNum >> 8);
    ba.append(crcNum & 0xFF);

    m_serialPort->write(ba);

}

void RS485::slotRecvAll()
{
    QByteArray byteArray = m_serialPort->readAll();
    m_recvByteArray.append(byteArray);

    if (m_recvByteArray.at(0) != m_hostNumber)
        m_recvByteArray.clear();
    //qDebug("************************************");
    //qDebug()<<"recv : "<<m_recvByteArray;

    int size = m_recvByteArray.size();
    if (size == 8) {
        if (m_recvByteArray.at(0) == m_hostNumber) {
            quint16 crcNum = dataCRC16((uchar*)m_recvByteArray.left(6).data(),m_recvByteArray.size()-2);
            if (((crcNum >> 8) != (uchar)m_recvByteArray.at(CRC_L)) && ((crcNum & 0xFF) != (uchar)m_recvByteArray.at(CRC_H))) {
                m_recvByteArray.clear();
                return;
            }
            uint code = m_recvByteArray.at(1);//功能码
            int  regAddr = (uchar)m_recvByteArray.at(3);//寄存器地址1
            switch (code) {
            case 0x03:
                if (regAddr == 0x34) {
                    sendData(0x03,0x02334,m_mainPower);//mainpower
                } else if(regAddr == 0x35) {
                    sendData(0x03,0x02335,m_backPower);//backPower
                } else {
                    quint16 startaddr = (uchar)m_recvByteArray.at(2) << 8 | (uchar)m_recvByteArray.at(3);
                    quint16 regCount  = (uchar)m_recvByteArray.at(4) << 8 | (uchar)m_recvByteArray.at(5);
                    //第一通道
                    if (startaddr <= STARTADDR + PASS_2_ADD) {
                        QByteArray dateArray;
                        startaddr = startaddr - STARTADDR;
                        for (int i = startaddr; i < startaddr + regCount;i++) {
                            dateArray.append(null_d);
                            dateArray.append(m_nodeArray[i].state);
                        }
                        sendData(0x03,dateArray.data(),dateArray.size());
                    } else {
                        //第二通道
                        QByteArray dateArray;
                        startaddr = startaddr - STARTADDR - PASS_2_ADD;
                        for (int i = startaddr; i < startaddr + regCount;i++) {
                            dateArray.append(null_d);
                            dateArray.append(m_nodeArray[i].state);
                        }
                        sendData(0x03,dateArray.data(),dateArray.size());
                    }
                }
                break;
            case 0x06:
            {

                QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                switch (regAddr) {
                case 0x29://time-year
                {
                    QString year = dateTime.left(4);;
                    int yy = year.toInt() - 2000;
                    sendData(0x06,0x2329,yy);
                }
                    break;
                case 0x2A://time-month
                {
                    QString month = dateTime.mid(5,2);
                    int MM = month.toInt();
                    sendData(0x06,0x0232A,MM);
                }
                    break;
                case 0x2B://time-date
                {
                    QString date = dateTime.mid(8,2);
                    int dd = date.toInt();
                    sendData(0x06,0x0232B,dd);
                }
                    break;
                case 0x2C://time-hour
                {
                    QString hour = dateTime.mid(10,2);
                    int hh = hour.toInt();
                    sendData(0x06,0x0232C,hh);
                }
                    break;
                case 0x2D://time-minute
                {
                    QString minute = dateTime.mid(13,2);
                    int mm = minute.toInt();
                    sendData(0x06,0x0232D,mm);
                }
                    break;
                case 0x2E://second
                {
                    QString second = dateTime.right(2);
                    int ss = second.toInt();
                    sendData(0x06,0x0232E,ss);
                }
                    break;
                case 0x32://reset
                    emit sigReset();
                    m_serialPort->write(m_recvByteArray);
                    break;
                case 0x33://mute
                    emit sigMute();
                    m_serialPort->write(m_recvByteArray);
                    break;
                }
            }
                break;
            }
        }
        m_recvByteArray.clear();
    } else if (size == BUFFSIZE) {
        quint16 crcNum = dataCRC16((uchar*)m_recvByteArray.left(13).data(),size-2);

        if (((crcNum >> 8) != (uchar)m_recvByteArray.at(13)) && ((crcNum & 0xFF) != (uchar)m_recvByteArray.at(14))) {
            return;
        }
        //set-time
        //SYSTEMTIME st;
        //GetSystemTime(&st);
        //st.wYear   = m_recvByteArray.at(7)+ 2000;
        //st.wMonth  = m_recvByteArray.at(8);
        //st.wDay    = m_recvByteArray.at(9);
        //st.wHour   = m_recvByteArray.at(10);
        //st.wMinute = m_recvByteArray.at(11);
        //st.wSecond = m_recvByteArray.at(12);
        //成功
        //if (SetLocalTime(&st) == true) {
        //    sendData(0x10,0x2329,0x03);
        //} else {
        //    QByteArray dateArray;
        //    dateArray.append(0x01);
        //    sendData(0x09,dateArray.data(),dateArray.size());
        //}
        m_recvByteArray.clear();
    }

    if (size > BUFFSIZE) {
        m_recvByteArray.clear();
    }
    if (size > 15) {
        m_recvByteArray.clear();
    }

}

void RS485::slotTimeOut()
{
    m_recvByteArray = m_serialPort->readAll();
    //qDebug("************************************");
    //qDebug()<<"recv : "<<m_recvByteArray;
    int size = m_recvByteArray.size();
    if (size == 8) {
        if (m_recvByteArray.at(0) == m_hostNumber) {
            quint16 crcNum = dataCRC16((uchar*)m_recvByteArray.left(6).data(),m_recvByteArray.size()-2);
            if (((crcNum >> 8) != (uchar)m_recvByteArray.at(CRC_L)) && ((crcNum & 0xFF) != (uchar)m_recvByteArray.at(CRC_H))) {
                m_recvByteArray.clear();
                return;
            }
            uint code = m_recvByteArray.at(1);//功能码
            int  regAddr = (uchar)m_recvByteArray.at(3);//寄存器地址1
            switch (code) {
            case 0x03:
                if (regAddr == 0x34) {
                    sendData(0x03,0x02334,m_mainPower);//mainpower
                } else if(regAddr == 0x35) {
                    sendData(0x03,0x02335,m_backPower);//backPower
                } else {
                    quint16 startaddr = (uchar)m_recvByteArray.at(2) << 8 | (uchar)m_recvByteArray.at(3);
                    quint16 regCount  = (uchar)m_recvByteArray.at(4) << 8 | (uchar)m_recvByteArray.at(5);
                    //第一通道
                    if (startaddr <= STARTADDR + PASS_2_ADD) {
                        QByteArray dateArray;
                        startaddr = startaddr - STARTADDR;
                        for (int i = startaddr; i < startaddr + regCount;i++) {
                            dateArray.append(null_d);
                            dateArray.append(m_nodeArray[i].state);
                        }
                        sendData(0x03,dateArray.data(),dateArray.size());
                    } else {
                        //第二通道
                        QByteArray dateArray;
                        startaddr = startaddr - STARTADDR - PASS_2_ADD;
                        for (int i = startaddr; i < startaddr + regCount;i++) {
                            dateArray.append(null_d);
                            dateArray.append(m_nodeArray[i].state);
                        }
                        sendData(0x03,dateArray.data(),dateArray.size());
                    }
                }
                break;
            case 0x06:
            {

                QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                switch (regAddr) {
                case 0x29://time-year
                {
                    QString year = dateTime.left(4);;
                    int yy = year.toInt() - 2000;
                    sendData(0x06,0x2329,yy);
                }
                    break;
                case 0x2A://time-month
                {
                    QString month = dateTime.mid(5,2);
                    int MM = month.toInt();
                    sendData(0x06,0x0232A,MM);
                }
                    break;
                case 0x2B://time-date
                {
                    QString date = dateTime.mid(8,2);
                    int dd = date.toInt();
                    sendData(0x06,0x0232B,dd);
                }
                    break;
                case 0x2C://time-hour
                {
                    QString hour = dateTime.mid(10,2);
                    int hh = hour.toInt();
                    sendData(0x06,0x0232C,hh);
                }
                    break;
                case 0x2D://time-minute
                {
                    QString minute = dateTime.mid(13,2);
                    int mm = minute.toInt();
                    sendData(0x06,0x0232D,mm);
                }
                    break;
                case 0x2E://second
                {
                    QString second = dateTime.right(2);
                    int ss = second.toInt();
                    sendData(0x06,0x0232E,ss);
                }
                    break;
                case 0x32://reset
                    emit sigReset();
                    m_serialPort->write(m_recvByteArray);
                    break;
                case 0x33://mute
                    emit sigMute();
                    m_serialPort->write(m_recvByteArray);
                    break;
                }
            }
                break;
            }
        }
        m_recvByteArray.clear();
    } else if (size == BUFFSIZE) {
        quint16 crcNum = dataCRC16((uchar*)m_recvByteArray.left(13).data(),size-2);

        if (((crcNum >> 8) != (uchar)m_recvByteArray.at(13)) && ((crcNum & 0xFF) != (uchar)m_recvByteArray.at(14))) {
            return;
        }
        //set-time
        //SYSTEMTIME st;
        //GetSystemTime(&st);
        //st.wYear   = m_recvByteArray.at(7)+ 2000;
        //st.wMonth  = m_recvByteArray.at(8);
        //st.wDay    = m_recvByteArray.at(9);
        //st.wHour   = m_recvByteArray.at(10);
        //st.wMinute = m_recvByteArray.at(11);
        //st.wSecond = m_recvByteArray.at(12);
        //成功
        //if (SetLocalTime(&st) == true) {
        //    sendData(0x10,0x2329,0x03);
        //} else {
        //    QByteArray dateArray;
        //    dateArray.append(0x01);
        //    sendData(0x09,dateArray.data(),dateArray.size());
        //}

        int Year   = m_recvByteArray.at(YEAR)+ 2000;
        int Month  = m_recvByteArray.at(MONTH);
        int Day    = m_recvByteArray.at(DAY);
        int Hour   = m_recvByteArray.at(HOUR);
        int Minute = m_recvByteArray.at(MINUTE);
        int Second = m_recvByteArray.at(SECOND);
        QString YearStr   = QString::number(Year);
        QString MonthStr  = QString::number(Month);
        QString DayStr    = QString::number(Day);
        QString HourStr   = QString::number(Hour);
        QString MinuteStr = QString::number(Minute);
        QString SecondStr = QString::number(Second);
        QString dateTime = YearStr+"/"+MonthStr+"/"+DayStr+" "+HourStr+":"+MinuteStr+":"+SecondStr;
        QString cmd = QStringLiteral("date -s ") + dateTime;
        QProcess::startDetached(cmd);
        QProcess::startDetached("hwclock -w"); // 同步系统时间
        QProcess::startDetached("sync"); // 保存配置
        m_recvByteArray.clear();
    }

    if (size > BUFFSIZE) {
        m_recvByteArray.clear();
    }
    if (size > 15) {
        m_recvByteArray.clear();
    }

}

void RS485::slotNodeState(QList<int> dataList)
{
    int pass  = dataList.at(0);
    int canId = dataList.at(1);
    int state = dataList.at(2);

    switch (state) {
    case GlobalData::NodeNormal:     //正常
        state = RS485::NodeNormal;
        break;
    case GlobalData::NodeOverCurrent://过流
        state = RS485::NodeOverCurrent;
        break;
    case GlobalData::NodeLackPhase:  //缺相
        state = RS485::NodeLackPhase;
        break;
    case GlobalData::NodeOverVoltage://过压
        state = RS485::NodeOverVoltage;
        break;
    case GlobalData::NodeUnderVoltage://欠压
        state = RS485::NodeUnderVoltage;
        break;
    case GlobalData::NodePowerLost:   //供电中断
        state = RS485::NodePowerLost;
        break;
    case GlobalData::NodeCanBusError: //通讯中断
        state = RS485::NodeCanBusError;
        break;
    }
    int CANID = canId - 1;
    if (CANID > 255)
        return;
    if (PASS_1 == pass) {
        m_nodeArray[CANID].state = state;
    } else if (PASS_2 == pass) {
        m_nodeArray[PASS_2_ADD + CANID].state = state;
    }

    //qDebug("******************");
    //qDebug()<<"pass  : "<<pass;
    //qDebug()<<"canId : "<<canId;
    //qDebug()<<"state : "<<state;
}

void RS485::slotPowerState(uchar mainPower, uchar backPower)
{
    m_mainPower = mainPower;
    m_backPower = backPower;
    if (m_mainPower == 1) {
        m_mainPower = 0;
    } else {
        m_mainPower = 2;
    }

    if (m_backPower == 1) {
        m_backPower = 0;
    } else {
        m_backPower = 2;
    }

}
