#include "cannetudp.h"
#include <QDebug>
#include <QThread>
#include "SQLite/sqlite.h"
#define DATASIZE    13
#define DATAAREA    8
#define CAN_CMD     0
#define CAN_TYPE    1
#define CAN_STATE   2
#define CAN_RD_L    3
#define CAN_RD_H    4
#define CAN_AD_L    5
#define CAN_AH_BL   6
#define CAN_BD_L    7

#define CMD_RESET   0x05

#define MOD_UREG    0//未注册
#define MOD_LEAK    2//漏电
#define MOD_TEMP    3//温度
#define MOD_EARC    4//电弧
CanNetUDP::CanNetUDP(QObject *parent) : QObject(parent)
{

}

CanNetUDP::~CanNetUDP()
{
    delete m_dataTimer;
    delete m_udpSocket;
}

CanNetUDP::CanNetUDP(const QHostAddress targetIP, const QHostAddress hostIP)
{
    m_udpSocket  = new QUdpSocket;
    m_targetIP   = targetIP; //目标地址
    m_hostIP     = hostIP;
}

void CanNetUDP::confCanNetUDP(quint16 targetPort, quint16 hostPort, int currentPass, int testTimes)
{
    m_testTimes  = testTimes;
    m_nodeIndex  = 0;
    m_currentPass= currentPass;

    QSqlDatabase db = SQLite::openConnection();
    m_nodeInfoList = SQLite::getNodeList(db,currentPass);
    SQLite::closeConnection(db);
    m_nodeInfoCount = m_nodeInfoList.count();

    m_targetPort = targetPort; //目标端口
    m_hostPort   = hostPort;
    //绑定本地地址和监听端口
    m_udpSocket->bind(m_hostIP,m_hostPort);

    for (int i= 0; i < m_nodeInfoCount;i++) {
        m_nodeTimesList.append(0);
    }


    m_dataTimer = new QTimer;
    connect(m_dataTimer,SIGNAL(timeout()),this,SLOT(slotTimeOut()));
    m_dataTimer->start(200);
}

QByteArray CanNetUDP::confCanInquiryData(uint canID, uint size)
{
    QByteArray byteArray;
    char dataHead = 32;
    dataHead |= size;
    char dataNull = 0;
    char dataCMD = 0x01;
    byteArray.append(dataHead);
    byteArray.append(dataNull);
    byteArray.append(dataNull);
    byteArray.append(dataNull);
    byteArray.append(canID);
    byteArray.append(dataCMD);
    byteArray.append(dataNull);
    byteArray.append(dataNull);
    byteArray.append(dataNull);
    byteArray.append(dataNull);
    byteArray.append(dataNull);
    byteArray.append(dataNull);
    byteArray.append(dataNull);
    return byteArray;
}

QByteArray CanNetUDP::readData()
{
    QByteArray recvData;
    while (m_udpSocket->hasPendingDatagrams()) {
        recvData.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(recvData.data(), recvData.size());
    }
    return recvData;
}

void CanNetUDP::sendDate(QByteArray byteArray)
{
    m_udpSocket->writeDatagram(byteArray,m_targetIP,m_targetPort);
}

void CanNetUDP::workUnit(int currentNode, int canID, int nodeType)
{
    QByteArray sendData = confCanInquiryData(canID,1);
    //发送数据
    sendDate(sendData);
    QThread::msleep(100);
    //读取数据
    QByteArray recvData = readData();

    if (DATASIZE == recvData.size()) {
        m_nodeTimesList[currentNode] = 0;
        int pCanID = recvData.at(3);
        pCanID = pCanID << 8 | recvData.at(4);//CAN帧ID
        recvData = recvData.right(DATAAREA);//CAN数据
        if (pCanID == canID) {
            canDataParse(currentNode, pCanID, recvData);
        }
    } else{
        if (m_nodeTimesList.at(currentNode) > m_testTimes) {
            m_nodeTimesList[currentNode] = 0;
            QByteArray recvData;
            for (int i = 0; i < recvData.count(); i++) {
                recvData.append((char)0);
            }
            recvData[CAN_TYPE] = nodeType;
            recvData[CAN_STATE]= 3;
            //recvData = recvData.right(DATAAREA);//CAN数据
            canDataParse(currentNode, canID, recvData);
        }
        m_nodeTimesList[currentNode]++;
    }
}

void CanNetUDP::canDataParse(int nodeIndex, uint canID, QByteArray recvData)
{
    QList<int> tempList;    tempList.clear();
    QList<int> leakList;    leakList.clear();
    QList<int> earcList;    earcList.clear();
    QList<int> uregList;    uregList.clear();

    int pNodeCanID = canID;
    int pNodeType  = recvData[CAN_TYPE];
    int pNodeState = recvData[CAN_STATE];

    switch (pNodeType) {
    case MOD_LEAK: {
        //实时数据
        int curLeak  = recvData[CAN_RD_H]<<8;//高位数
        curLeak |= recvData[CAN_RD_L];//低位数
        //报警数值
        int alarmLeak = recvData[CAN_AH_BL] >> 4;//取出高4位数
        alarmLeak <<= 8;
        alarmLeak |= recvData[CAN_AD_L];//低位数
        //固有漏电
        int baseLeak =  recvData[CAN_AH_BL] & 0xF;//取出低4位数
        baseLeak <<= 8;
        baseLeak |= recvData[CAN_BD_L];//低位数
        leakList<<nodeIndex<<m_currentPass<<pNodeCanID<<pNodeType<<pNodeState<<curLeak<<alarmLeak<<baseLeak;
        emit sigSendCanData(leakList);
    }
        break;
    case MOD_TEMP: {
        int curTemp   = recvData[CAN_RD_L];
        int alarmTemp = recvData[CAN_AD_L];
        tempList<<nodeIndex<<m_currentPass<<pNodeCanID<<pNodeType<<pNodeState<<curTemp<<alarmTemp<<0;
        emit sigSendCanData(tempList);
    }
        break;
    case MOD_EARC:
        earcList<<nodeIndex<<m_currentPass<<pNodeCanID<<pNodeType<<pNodeState<<0<<0<<0;
        emit sigSendCanData(earcList);
        break;
    case MOD_UREG:
        uregList<<nodeIndex<<m_currentPass<<pNodeCanID<<pNodeType<<pNodeState<<0<<0<<0;
        emit sigSendCanData(uregList);
        break;
    }

}

void CanNetUDP::timerPause()
{
    m_dataTimer->stop();
    QByteArray byteArray;
    char dataHead = 32;
    dataHead |= 8;
    char dataNull = 0;
    char dataCMD = CMD_RESET;
    byteArray.append(dataHead);
    byteArray.append(dataNull);
    byteArray.append(dataNull);
    byteArray.append(dataNull);
    byteArray.append(dataNull);
    byteArray.append(dataCMD);
    byteArray.append(dataNull);
    byteArray.append(dataNull);
    byteArray.append(dataNull);
    byteArray.append(dataNull);
    byteArray.append(dataNull);
    byteArray.append(dataNull);
    byteArray.append(dataNull);
    sendDate(byteArray);
    emit sigThreadPaused(m_currentPass);
}

void CanNetUDP::timerRestart()
{
    m_dataTimer->start();
}

void CanNetUDP::slotTimeOut()
{
    if (m_nodeInfoCount == 0)
        return;

    if (m_nodeIndex < m_nodeInfoCount) {
        int pCanID  = m_nodeInfoList.value(m_nodeIndex).first;
        int pNodeType= m_nodeInfoList.value(m_nodeIndex).second;
        workUnit(m_nodeIndex,pCanID,pNodeType);
        m_nodeIndex++;
        if (m_nodeIndex == m_nodeInfoCount) {
            m_nodeIndex = 0;
        }
    }
}



















