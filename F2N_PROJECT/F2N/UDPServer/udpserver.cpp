#include "udpserver.h"
#include <QNetworkInterface>
UDPServer::UDPServer(QObject *parent) : QObject(parent)
{
    m_udpSocket = new QUdpSocket(this);
}

UDPServer::~UDPServer()
{
    delete m_udpSocket;
}

void UDPServer::initConf(const QHostAddress targetHost, quint16 targetPort)
{
    m_targetPort = targetPort; //目标端口
    m_targetHost = targetHost; //目标地址
    m_udpSocket->bind(targetHost,m_targetPort);
    connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(slotReadReady()));
}

QString UDPServer::getHostIPAddr()
{
    QString strIpAddress;
    QList<QHostAddress> ipAddrList = QNetworkInterface::allAddresses();
    // 获取第一个本主机的IPv4地址
    int listSize = ipAddrList.size();
    for (int i = 0; i < listSize; ++i)
    {
        if (ipAddrList.at(i) != QHostAddress::LocalHost && ipAddrList.at(i).toIPv4Address())
        {
            strIpAddress = ipAddrList.at(i).toString();
            break;
        }
    }     // 如果没有找到，则以本地IP地址为IP
    if (strIpAddress.isEmpty())
        strIpAddress = QHostAddress(QHostAddress::LocalHost).toString();
    return strIpAddress;
}

void UDPServer::sendDate(QByteArray byteArray)
{
    m_udpSocket->writeDatagram(byteArray,m_targetHost,m_targetPort);
}

void UDPServer::slotReadReady()
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
        default:
            break;
        }
    }
}

void UDPServer::slotSendCanData(int index, int pass, int canId, int type, int state, QList<int> voltageList, QList<qreal> currentList,QString area)
{
    QByteArray byteArray;
    byteArray.append((uint)(index)>>8);
    byteArray.append((uint)(index) & 0xFF);
    byteArray.append((uint)(pass));
    byteArray.append((uint)(canId)>>8);
    byteArray.append((uint)(canId) & 0xFF);
    byteArray.append((uint)(state));
    byteArray.append((uint)(type));

    uint av1_h = (uint)voltageList.value(0) >> 8;
    uint av1_l = (uint)voltageList.value(0) & 0xFF;
    uint bv1_h = (uint)voltageList.value(1) >> 8;
    uint bv1_l = (uint)voltageList.value(1) & 0xFF;
    uint cv1_h = (uint)voltageList.value(2) >> 8;
    uint cv1_l = (uint)voltageList.value(2) & 0xFF;
    uint av2_h = (uint)voltageList.value(3) >> 8;
    uint av2_l = (uint)voltageList.value(3) & 0xFF;
    uint bv2_h = (uint)voltageList.value(4) >> 8;
    uint bv2_l = (uint)voltageList.value(4) & 0xFF;
    uint cv2_h = (uint)voltageList.value(5) >> 8;;
    uint cv2_l = (uint)voltageList.value(5) & 0xFF;

    byteArray.append(av1_h);
    byteArray.append(av1_l);
    byteArray.append(bv1_h);
    byteArray.append(bv1_l);
    byteArray.append(cv1_h);
    byteArray.append(cv1_l);

    byteArray.append(av2_h);
    byteArray.append(av2_l);
    byteArray.append(bv2_h);
    byteArray.append(bv2_l);
    byteArray.append(cv2_h);
    byteArray.append(cv2_l);

    QString aiStr = QString::number(currentList.value(0));
    QString biStr = QString::number(currentList.value(1));
    QString ciStr = QString::number(currentList.value(2));

    byteArray.append(aiStr.left(aiStr.indexOf(".")).toUInt());
    byteArray.append(aiStr.right(1).toUInt());
    byteArray.append(biStr.left(biStr.indexOf(".")).toUInt());
    byteArray.append(biStr.right(1).toUInt());
    byteArray.append(ciStr.left(ciStr.indexOf(".")).toUInt());
    byteArray.append(ciStr.right(1).toUInt());

    QByteArray nodeAddrBy = area.toUtf8().data();
    byteArray.append(nodeAddrBy);
    sendDate(byteArray);
}
