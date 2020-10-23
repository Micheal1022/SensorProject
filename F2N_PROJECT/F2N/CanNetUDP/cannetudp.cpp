#include "cannetudp.h"
#include <QThread>
#include "GlobalData/globaldata.h"
#define DATASIZE    13
#define DATAAREA    8
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
    m_dataTimer = new QTimer;
    m_udpSocket = new QUdpSocket;
    m_targetIP  = targetIP; //目标地址
    m_hostIP    = hostIP;

}

void CanNetUDP::confCanNetUDP(quint16 targetPort, quint16 hostPort, QList<int> nodeIDList, int currentPass, int testTimes)
{
    m_testTimes  = testTimes;
    m_nodeIndex= 0;
    m_currentPass= currentPass;

    m_nodeIDList = nodeIDList;
    m_nodeIDCount= nodeIDList.count();
    m_targetPort = targetPort; //目标端口
    m_hostPort   = hostPort;
    //绑定本地地址和监听端口
    m_udpSocket->bind(m_hostIP,m_hostPort);

    for (int i= 0; i < m_nodeIDCount;i++) {
        m_nodeTimesList.append(0);
    }

    connect(m_dataTimer,SIGNAL(timeout()),this,SLOT(slotTimeOut()));
    m_dataTimer->start(250);
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

void CanNetUDP::sendDate(QByteArray byteArray)
{
    m_udpSocket->writeDatagram(byteArray,m_targetIP,m_targetPort);
}

void CanNetUDP::workUnit(int currentNode,uint canID)
{
    QByteArray sendData = confCanInquiryData(canID,1);
    //发送数据
    sendDate(sendData);
    QThread::msleep(100);
    //读取数据
    QByteArray recvData;
    while (m_udpSocket->hasPendingDatagrams()) {
        recvData.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(recvData.data(), recvData.size());
    }

    if (DATASIZE == recvData.size()) {
        m_nodeTimesList[currentNode] = 0;
        uint pCanID = recvData.at(3);
        pCanID = pCanID << 8 | recvData.at(4);//CAN帧ID
        recvData = recvData.right(DATAAREA);//CAN数据
        if (pCanID == canID) {
            canDataParse(currentNode, pCanID, recvData);
        }
    } else{
        if (m_nodeTimesList.at(currentNode) > m_testTimes) {
            m_nodeTimesList[currentNode] = 0;
            QByteArray recvData;recvData.resize(DATASIZE);
            recvData[0] = GlobalData::MODE_UNKWN;
            recvData[1] = GlobalData::NodeCanBusError;
            //recvData = recvData.right(DATAAREA);//CAN数据
            canDataParse(currentNode, canID, recvData);
        }
        m_nodeTimesList[currentNode]++;
    }
}

void CanNetUDP::canDataParse(int currentNode, uint canID, QByteArray recvData)
{
    qDebug()<<"currentNode =====>"<<currentNode;
    qDebug()<<"canID       =====>"<<canID;
    qDebug()<<"recvData    =====>"<<recvData;
    int nodeType,nodeState,nodeAV_1,nodeBV_1,nodeCV_1,nodeAV_2,nodeBV_2,nodeCV_2;
    nodeType = nodeState = nodeAV_1 = nodeBV_1 = nodeCV_1 = nodeAV_2 = nodeBV_2 = nodeCV_2 = 0;
    qreal nodeAI_1,nodeBI_1,nodeCI_1;
    nodeAI_1 = nodeBI_1 = nodeCI_1 = 0;

    if (recvData.at(1) != GlobalData::NodeCanBusError) {
        //探测器类型
        nodeType = recvData.at(0);
    }
    //探测器状态
    nodeState = recvData.at(1);

    QList<int> voltageList;
    voltageList.clear();

    QList<qreal> currentList;
    currentList.clear();

    QList<int> dataList;
    dataList<<m_currentPass<<canID<<nodeState;
    emit sigSendRS485Data(dataList);

    switch (nodeType) {
    case GlobalData::MODE_UNKWN:
        nodeAV_1 = 0;
        nodeBV_1 = 0;
        nodeCV_1 = 0;
        nodeAV_2 = 0;
        nodeBV_2 = 0;
        nodeCV_2 = 0;
        voltageList<<nodeAV_1<<nodeBV_1<<nodeCV_1<<nodeAV_2<<nodeBV_2<<nodeCV_2;
        currentList<<0.0<<0.0<<0.0;
        emit sigSendCanData(currentNode,m_currentPass,canID,nodeType,nodeState,voltageList,currentList);

        break;
    case GlobalData::MODE_DVA:
        nodeAV_1 = recvData[2];
        nodeBV_1 = 0;
        nodeCV_1 = 0;
        nodeAV_2 = 0;
        nodeBV_2 = 0;
        nodeCV_2 = 0;
        voltageList<<nodeAV_1<<nodeBV_1<<nodeCV_1<<nodeAV_2<<nodeBV_2<<nodeCV_2;
        currentList<<recvData[3]<<0.0<<0.0;
        emit sigSendCanData(currentNode,m_currentPass,canID,nodeType,nodeState,voltageList,currentList);
        break;

    case GlobalData::MODE_V:
    case GlobalData::MODE_V3:
    case GlobalData::MODE_VN3:
        nodeAV_1 = recvData[2]*2;
        nodeBV_1 = recvData[3]*2;
        nodeCV_1 = recvData[4]*2;
        nodeAV_2 = recvData[5]*2;
        nodeBV_2 = recvData[6]*2;
        nodeCV_2 = recvData[7]*2;
        voltageList<<nodeAV_1<<nodeBV_1<<nodeCV_1<<nodeAV_2<<nodeBV_2<<nodeCV_2;
        currentList<<0.0<<0.0<<0.0;
        emit sigSendCanData(currentNode,m_currentPass,canID,nodeType,nodeState,voltageList,currentList);
        break;

    case GlobalData::MODE_VA3:
    case GlobalData::MODE_VAN3:

        nodeAV_1 =  recvData[2]*2;
        nodeBV_1 =  recvData[3]*2;
        nodeCV_1 =  recvData[4]*2;

        nodeAI_1 = (recvData[5] >> 4) + ((qreal)(recvData[5] & 0xF))/10;
        nodeBI_1 = (recvData[6] >> 4) + ((qreal)(recvData[6] & 0xF))/10;
        nodeCI_1 = (recvData[7] >> 4) + ((qreal)(recvData[7] & 0xF))/10;
        voltageList<<nodeAV_1<<nodeBV_1<<nodeCV_1<<0<<0<<0;
        currentList<<nodeAI_1<<nodeBI_1<<nodeCI_1;
        emit sigSendCanData(currentNode,m_currentPass,canID,nodeType,nodeState,voltageList,currentList);
        break;

    case GlobalData::MODE_VA:

        nodeAV_1 =  recvData[2]*2;
        nodeBV_1 =  recvData[4]*2;
        nodeCV_1 =  recvData[6]*2;

        nodeAI_1 = (recvData[3] >> 4) + ((qreal)(recvData[3] & 0xF))/10;
        nodeBI_1 = (recvData[5] >> 4) + ((qreal)(recvData[5] & 0xF))/10;
        nodeCI_1 = (recvData[7] >> 4) + ((qreal)(recvData[7] & 0xF))/10;

        voltageList<<nodeAV_1<<nodeBV_1<<nodeCV_1<<0<<0<<0;
        currentList<<nodeAI_1<<nodeBI_1<<nodeCI_1;

        emit sigSendCanData(currentNode,m_currentPass,canID,nodeType,nodeState,voltageList,currentList);
        break;

    case GlobalData::MODE_2VAN3:
        nodeAV_1 = recvData[2]*2;
        nodeBV_1 = recvData[3]*2;
        nodeCV_1 = recvData[4]*2;
        nodeAV_2 = recvData[5]*2;
        nodeBV_2 = recvData[6]*2;
        nodeCV_2 = recvData[7]*2;
        QByteArray recvData;
        while (m_udpSocket->hasPendingDatagrams()) {
            recvData.resize(m_udpSocket->pendingDatagramSize());
            m_udpSocket->readDatagram(recvData.data(), recvData.size());
        }
        recvData = recvData.right(8);
        if (DATASIZE == recvData.size()) {
            nodeAI_1 = (recvData[4] >> 4) + ((qreal)(recvData[4] & 0xF))/10;
            nodeBI_1 = (recvData[5] >> 4) + ((qreal)(recvData[5] & 0xF))/10;
            nodeCI_1 = (recvData[6] >> 4) + ((qreal)(recvData[6] & 0xF))/10;
        }

        voltageList<<nodeAV_1<<nodeBV_1<<nodeCV_1<<nodeAV_2<<nodeBV_2<<nodeCV_2;
        currentList<<nodeAI_1<<nodeBI_1<<nodeCI_1;
        emit sigSendCanData(currentNode,m_currentPass,canID,nodeType,nodeState,voltageList,currentList);
        break;
    }

}

void CanNetUDP::slotReadData()
{
    QByteArray recvData;
    while (m_udpSocket->hasPendingDatagrams()) {
        recvData.resize(m_udpSocket->pendingDatagramSize());
        m_udpSocket->readDatagram(recvData.data(), recvData.size());
    }
}

void CanNetUDP::slotTimeOut()
{
    if (m_nodeIDCount == 0)
        return;

    if (m_nodeIndex < m_nodeIDCount) {
        uint pCanId = m_nodeIDList.at(m_nodeIndex);
        workUnit(m_nodeIndex,pCanId);
        m_nodeIndex++;
    } else {
        m_nodeIndex = 0;
    }
}




















