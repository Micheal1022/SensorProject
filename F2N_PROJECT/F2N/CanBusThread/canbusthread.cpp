#include "canbusthread.h"
#include <QDebug>
#include <QApplication>
#include "SQLite/sqlite.h"
#define PASS_1 0
#define PASS_2 1
#define TIMES  10
DWORD CanBusThread::m_devType = USBCAN;

CanBusThread::CanBusThread()
{

}

CanBusThread::CanBusThread(int pass, QList<int> nodeListPass)
{
    m_nodeListPass = nodeListPass;
    m_passNodeCount = m_nodeListPass.count();
    m_dropTimes = 0;
    m_currentNode = 0;
    m_currentPass = pass;
    m_CANInd = pass-1;

    if (m_passNodeCount > 0) {
        if(initCanBusDev(m_CANInd))
            qDebug()<<QString("PASS_%1").arg(m_CANInd)<<" InitCanBusDev OK!";
        else
            qDebug()<<QString("PASS_%1").arg(m_CANInd)<<" InitCanBusDev ERROR!";
        qDebug()<<"************************";
    }
}

CanBusThread::~CanBusThread()
{
    requestInterruption();
    closeCanBusDev();
    quit();
    wait();
    qDebug()<<"~CanBusThread()";
}

bool CanBusThread::openCanBusDev()
{
    if (OpenDevice(m_devType,0,0) != STATUS_OK) {
        qDebug()<<"OpenCanDev USBCAN ERROR";
        return false;
    }
    qDebug()<<"OpenCanDev USBCAN OK";
    return true;
}

bool CanBusThread::closeCanBusDev()
{
    if (CloseDevice(m_devType,0) != STATUS_OK) {
        qDebug()<<"CloseCanDev USBCAN ERROR";
        return false;
    }
    qDebug()<<"CloseCanDev USBCAN OK";
    return true;
}

bool CanBusThread::initCanBusDev(DWORD CANIndex)
{
    INIT_CONFIG init_config;
    init_config.AccCode = 0;
    init_config.AccMask = 0xffffff;
    init_config.Filter  = 0;
    init_config.Timing0 = 0x31;
    init_config.Timing1 = 0x1c;
    init_config.Mode    = 0;


    if (InitCAN(m_devType,0,CANIndex,&init_config) == STATUS_OK) {
        qDebug()<<QString("PASS_%1").arg(CANIndex)<<" InitCAN OK!";
    } else {
        qDebug()<<QString("PASS_%1").arg(CANIndex)<<" InitCAN ERROR!";
        return false;
    }

    if (StartCAN(m_devType,0,CANIndex) == STATUS_OK) {
        qDebug()<<QString("PASS_%1").arg(CANIndex)<<" StartCAN OK!";
    } else {
        qDebug()<<QString("PASS_%1").arg(CANIndex)<<" StartCAN ERROR!";
        return false;
    }
    return true;
}

CAN_OBJ CanBusThread::confFrame(uint canId)
{
    CAN_OBJ transmitFrameInfo;
    memset(&transmitFrameInfo,0,sizeof(transmitFrameInfo));
    transmitFrameInfo.SendType   = 1;
    transmitFrameInfo.RemoteFlag = 0;
    transmitFrameInfo.ExternFlag = 0;
    transmitFrameInfo.ID         = canId;
    transmitFrameInfo.DataLen    = 1;
    transmitFrameInfo.Data[0]    = 0x01;
    return transmitFrameInfo;
}

void CanBusThread::canDataParse(int currentNode,int devType,int CANInd,CAN_OBJ data)
{
    int nodeType,nodeState,nodeAV_1,nodeBV_1,nodeCV_1,nodeAV_2,nodeBV_2,nodeCV_2;
    nodeType = nodeState = nodeAV_1 = nodeBV_1 = nodeCV_1 = nodeAV_2 = nodeBV_2 = nodeCV_2 = 0;
    qreal nodeAI_1,nodeBI_1,nodeCI_1;
    nodeAI_1 = nodeBI_1 = nodeCI_1 = 0;

    if (data.Data[1] != NodeCanBusError) {
        nodeType = data.Data[0];
    }
    nodeState = data.Data[1];

    QList<int> voltageList;voltageList.clear();
    QList<qreal> currentList;currentList.clear();

    QList<int> dataList;
    dataList<<m_currentPass<<data.ID<<nodeState;
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
        emit sigSendCanData(currentNode,m_currentPass,data.ID,nodeType,nodeState,voltageList,currentList);

        break;
    case GlobalData::MODE_DVA:
        nodeAV_1 = data.Data[2];
        nodeBV_1 = 0;
        nodeCV_1 = 0;
        nodeAV_2 = 0;
        nodeBV_2 = 0;
        nodeCV_2 = 0;
        voltageList<<nodeAV_1<<nodeBV_1<<nodeCV_1<<nodeAV_2<<nodeBV_2<<nodeCV_2;
        currentList<<data.Data[3]<<0.0<<0.0;
        emit sigSendCanData(currentNode,m_currentPass,data.ID,nodeType,nodeState,voltageList,currentList);
        break;
    case GlobalData::MODE_V:
    case GlobalData::MODE_V3:
    case GlobalData::MODE_VN3:
        nodeAV_1 = data.Data[2]*2;
        nodeBV_1 = data.Data[3]*2;
        nodeCV_1 = data.Data[4]*2;
        nodeAV_2 = data.Data[5]*2;
        nodeBV_2 = data.Data[6]*2;
        nodeCV_2 = data.Data[7]*2;
        voltageList<<nodeAV_1<<nodeBV_1<<nodeCV_1<<nodeAV_2<<nodeBV_2<<nodeCV_2;
        currentList<<0.0<<0.0<<0.0;
        emit sigSendCanData(currentNode,m_currentPass,data.ID,nodeType,nodeState,voltageList,currentList);

        break;

    case GlobalData::MODE_VA3:
    case GlobalData::MODE_VAN3:

        nodeAV_1 =  data.Data[2]*2;
        nodeBV_1 =  data.Data[3]*2;
        nodeCV_1 =  data.Data[4]*2;

        nodeAI_1 = (data.Data[5] >> 4) + ((qreal)(data.Data[5] & 0xF))/10;
        nodeBI_1 = (data.Data[6] >> 4) + ((qreal)(data.Data[6] & 0xF))/10;
        nodeCI_1 = (data.Data[7] >> 4) + ((qreal)(data.Data[7] & 0xF))/10;
        voltageList<<nodeAV_1<<nodeBV_1<<nodeCV_1<<0<<0<<0;
        currentList<<nodeAI_1<<nodeBI_1<<nodeCI_1;
        emit sigSendCanData(currentNode,m_currentPass,data.ID,nodeType,nodeState,voltageList,currentList);

        break;
    case GlobalData::MODE_VA:

        nodeAV_1 =  data.Data[2]*2;
        nodeBV_1 =  data.Data[4]*2;
        nodeCV_1 =  data.Data[6]*2;
        nodeAI_1 = (data.Data[3] >> 4) + ((qreal)(data.Data[3] & 0xF))/10;
        nodeBI_1 = (data.Data[5] >> 4) + ((qreal)(data.Data[5] & 0xF))/10;
        nodeCI_1 = (data.Data[7] >> 4) + ((qreal)(data.Data[7] & 0xF))/10;

        voltageList<<nodeAV_1<<nodeBV_1<<nodeCV_1<<0<<0<<0;
        currentList<<nodeAI_1<<nodeBI_1<<nodeCI_1;

        emit sigSendCanData(currentNode,m_currentPass,data.ID,nodeType,nodeState,voltageList,currentList);
        break;

    case GlobalData::MODE_2VAN3:
        nodeAV_1 = data.Data[2]*2;
        nodeBV_1 = data.Data[3]*2;
        nodeCV_1 = data.Data[4]*2;
        nodeAV_2 = data.Data[5]*2;
        nodeBV_2 = data.Data[6]*2;
        nodeCV_2 = data.Data[7]*2;
        CAN_OBJ receiveFrameInfo;
        memset(&receiveFrameInfo,0,sizeof(receiveFrameInfo));
        if(Receive(devType,0,CANInd,&receiveFrameInfo,1,150) == 1)
        {
            nodeAI_1 = (receiveFrameInfo.Data[4] >> 4) + ((qreal)(receiveFrameInfo.Data[4] & 0xF))/10;
            nodeBI_1 = (receiveFrameInfo.Data[5] >> 4) + ((qreal)(receiveFrameInfo.Data[5] & 0xF))/10;
            nodeCI_1 = (receiveFrameInfo.Data[6] >> 4) + ((qreal)(receiveFrameInfo.Data[6] & 0xF))/10;
        }
        voltageList<<nodeAV_1<<nodeBV_1<<nodeCV_1<<nodeAV_2<<nodeBV_2<<nodeCV_2;
        currentList<<nodeAI_1<<nodeBI_1<<nodeCI_1;
        emit sigSendCanData(currentNode,m_currentPass,data.ID,nodeType,nodeState,voltageList,currentList);
        break;
    }

}

void CanBusThread::workUnit(int devType,int CANInd, int currentNode, QList<int> nodeList)
{
    for (int times = 0; times < TIMES; times++) {
        //ClearBuffer(m_devType,0,CANInd);
        msleep(200);
        uint ID = nodeList.value(currentNode);
        CAN_OBJ transmitFrameInfo = confFrame(ID);
        Transmit(devType,0,CANInd,&transmitFrameInfo,1);

        CAN_OBJ receiveFrameInfo;
        memset(&receiveFrameInfo,0,sizeof(receiveFrameInfo));
        msleep(150);
        uint recvNum = Receive(devType,0,CANInd,&receiveFrameInfo,1,0);

        if ((recvNum >= 1) && (transmitFrameInfo.ID == receiveFrameInfo.ID)) {
            canDataParse(currentNode,devType,CANInd,receiveFrameInfo);
            break;
        } else {
            if (times == TIMES-1) {
                receiveFrameInfo.ID = ID;
                receiveFrameInfo.Data[0] = MODE_UNKWN;
                receiveFrameInfo.Data[1] = NodeCanBusError;
                receiveFrameInfo.DataLen = 1;
                canDataParse(currentNode,devType,CANInd,receiveFrameInfo);
            }
        }
    }
}

void CanBusThread::run()
{
    if (m_passNodeCount == 0) {
        requestInterruption();
    }
    int currentNode = 0;
    while (!isInterruptionRequested()) {
        if (m_passNodeCount > 0) {

            if (currentNode < m_passNodeCount) {
                workUnit(m_devType,m_CANInd,currentNode,m_nodeListPass);
                currentNode++;
            } else {
                currentNode = 0;
                //巡检一轮结束后,复位CAN通道,重启启动CAN设备
                if (STATUS_ERR == CANReset()) {
                    CANReset();
                }
                if (STATUS_ERR == CANStart()) {
                    CANStart();
                }
            }
        }
    }
}

int CanBusThread::CANStart()
{
    sleep(2);
    if (StartCAN(m_devType,0,m_currentPass) == STATUS_OK) {
        qDebug()<<QString("PASS_%1").arg(m_currentPass)<<" StartCAN OK!";
        return STATUS_OK;
    } else {
        qDebug()<<QString("PASS_%1").arg(m_currentPass)<<" StartCAN ERROR!";
    }
    return STATUS_ERR;
}

int CanBusThread::CANReset()
{
    sleep(2);
    if (ResetCAN(m_devType,0,m_currentPass) == STATUS_OK) {
        qDebug()<<QString("PASS_%1").arg(m_currentPass)<<" ResetCAN OK!";
        return STATUS_OK;
    } else {
        qDebug()<<QString("PASS_%1").arg(m_currentPass)<<" ResetCAN ERROR!";
    }
    return STATUS_ERR;
}


void CanBusThread::slotReset()
{
    CAN_OBJ transmitFrameInfo;
    transmitFrameInfo.SendType   = 0;
    transmitFrameInfo.RemoteFlag = 0;
    transmitFrameInfo.ExternFlag = 0;
    transmitFrameInfo.ID      = 0;
    transmitFrameInfo.DataLen = 1;
    transmitFrameInfo.Data[0] = 0x05;
    Transmit(m_devType,0,m_currentPass,&transmitFrameInfo,1);
    CANReset();
    CANStart();
}


