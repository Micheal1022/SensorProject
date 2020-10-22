#include "updmgm.h"
#include <QDateTime>
#include <QDebug>
#include <QNetworkInterface>
#include "SQLite/sqlite.h"

#define INDEX      0
#define PASS       1
#define CANID      2
#define TYPE       3
#define STATE      4
#define CURVALUE   5
#define ALARMVALUE 6
#define BASEVALUE  7

UDPMgm::UDPMgm(QObject *parent) : QObject(parent)
{
    m_udpSocket = new QUdpSocket(this);
}

UDPMgm::~UDPMgm()
{

}

void UDPMgm::initConf(const QHostAddress &host, quint16 port)
{
    m_port = port;
    m_hostAddr = host;
    m_udpSocket->bind(host,m_port);
    connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(slotReadReady()));
}

void UDPMgm::nodeValue(QList<int> data)
{
    QByteArray byteArray;
    byteArray.append(data.at(INDEX) >> 8);
    byteArray.append(data.at(INDEX) & 0xFF);
    byteArray.append(data.at(PASS) + 1);
    byteArray.append(data.at(CANID) >> 8);
    byteArray.append(data.at(CANID) & 0xFF);
    byteArray.append(data.at(TYPE));
    byteArray.append(data.at(STATE));
    byteArray.append(data.at(CURVALUE) >> 8);
    byteArray.append(data.at(CURVALUE) & 0xFF);
    byteArray.append(data.at(ALARMVALUE) >> 8);
    byteArray.append(data.at(ALARMVALUE) & 0xFF);
    byteArray.append(data.at(BASEVALUE) >> 8);
    byteArray.append(data.at(BASEVALUE) & 0xFF);

    QSqlDatabase db = SQLite::openConnection();
    QString nodeAddr = SQLite::getNodeArea(db,QString::number(data.at(PASS)+1),QString::number(data.at(CANID)));
    SQLite::closeConnection(db);
    QByteArray nodeAddrBy = nodeAddr.toUtf8().data();
    byteArray.append(nodeAddrBy);

    sendDate(byteArray);
}

void UDPMgm::sendDate(QByteArray byteArray)
{
    qDebug()<<"byteArray : "<<byteArray;
    m_udpSocket->writeDatagram(byteArray,m_hostAddr,m_port);
}

void UDPMgm::slotReadReady()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(datagram.data(), datagram.size());
        switch (datagram.at(0)) {
        case 1://mute
            emit sigMute();
            break;
        case 2://reset
            emit sigReset();
            break;
        }
    }
}


QString UDPMgm::getHostIpAddress()
{
    QString strIpAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // 获取第一个本主机的IPv4地址
    int nListSize = ipAddressesList.size();
    for (int i = 0; i < nListSize; ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address()) {
            strIpAddress = ipAddressesList.at(i).toString();
            break;
        }
    }     // 如果没有找到，则以本地IP地址为IP
    if (strIpAddress.isEmpty())
        strIpAddress = QHostAddress(QHostAddress::LocalHost).toString();
    return strIpAddress;
}




