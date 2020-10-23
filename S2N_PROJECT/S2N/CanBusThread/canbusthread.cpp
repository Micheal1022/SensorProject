#include "canbusthread.h"
#include <QDebug>
#include "SQLite/sqlite.h"
//#define P_PRINT
//#define P_ALARM
//#define P_OFFLINE
#define TIMES  1

#define CAN_CMD     0
#define CAN_TYPE    1
#define CAN_STATE   2
#define CAN_RD_L    3
#define CAN_RD_H    4
#define CAN_AD_L    5
#define CAN_AH_BL   6
#define CAN_BD_L    7

#define CMD_RESET   0x05




CanBusThread::CanBusThread()
{


}

CanBusThread::CanBusThread(DWORD DeviceType, int currentPass)
{
    QSqlDatabase db = SQLite::openConnection();
    m_nodeList = SQLite::getNodeList(db,currentPass);
    SQLite::closeConnection(db);
    m_nodeCount = m_nodeList.count();
    m_reset = false;
    m_CANIndex    = currentPass-1;
    m_currentPass = currentPass;
    m_DeviceType  = DeviceType;
    initCanBusDev(m_DeviceType,m_CANIndex);
}

CanBusThread::~CanBusThread()
{
    requestInterruption();
    quit();
    wait();
}

void CanBusThread::run()
{
    //节点索引号
    m_nodeIndex = 0;
    m_offLineFlag = false;
    m_offLineTimes = 0;
    msleep(3000);
    while (1) {
        if (m_nodeIndex < m_nodeCount) {
            if (true == m_reset) {
                m_nodeIndex = 0;
                m_reset = false;
                ResetCAN(m_DeviceType,0,m_CANIndex);
                msleep(2000);
                StartCAN(m_DeviceType,0,m_CANIndex);
                emit sigThreadPaused(m_currentPass);
                msleep(3000);
            } else {
                int nodeID  = m_nodeList.value(m_nodeIndex).first;
                int nodeType= m_nodeList.value(m_nodeIndex).second;
                canWorkUnit(m_nodeIndex,nodeID,nodeType);
                m_nodeIndex++;

                if (m_nodeIndex == m_nodeCount) {
                    m_nodeIndex = 0;
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
}

bool CanBusThread::initCanBusDev(DWORD DeviceType, DWORD CANIndex)
{
    m_DeviceType = DeviceType;

    INIT_CONFIG init_config;
    init_config.AccCode = 0;
    init_config.AccMask = 0xffffff;
    init_config.Filter  = 0;
    init_config.Timing0 = 0x31;
    init_config.Timing1 = 0x1c;
    init_config.Mode    = 0;


    if (InitCAN(DeviceType,0,CANIndex,&init_config) == STATUS_OK) {
        qDebug()<<QString("PASS_%1").arg(CANIndex)<<" InitCAN OK!";
    } else {
        qDebug()<<QString("PASS_%1").arg(CANIndex)<<" InitCAN ERROR!";
        return false;
    }

    if (StartCAN(DeviceType,0,CANIndex) == STATUS_OK) {
        qDebug()<<QString("PASS_%1").arg(CANIndex)<<" StartCAN OK!";
    } else {
        qDebug()<<QString("PASS_%1").arg(CANIndex)<<" StartCAN ERROR!";
        return false;
    }
    return true;
}

void CanBusThread::threadPause()
{
    m_reset = true;
    CAN_OBJ transmitFrameInfo;
    memset(&transmitFrameInfo,0,sizeof(transmitFrameInfo));
    transmitFrameInfo.SendType   = 0;
    transmitFrameInfo.RemoteFlag = 0;
    transmitFrameInfo.ExternFlag = 0;
    transmitFrameInfo.ID         = 0;
    transmitFrameInfo.DataLen    = 1;
    transmitFrameInfo.Data[0]    = CMD_RESET;
    Transmit(m_DeviceType,0,m_CANIndex,&transmitFrameInfo,1);
}

void CanBusThread::threadRestart()
{
    m_reset = false;
}

void CanBusThread::controlAlarmData(quint32 ID, quint16 type ,quint16 alarm, quint16 base)
{
    CAN_OBJ transmitFrameInfo;
    memset(&transmitFrameInfo,0,sizeof(transmitFrameInfo));
    transmitFrameInfo.SendType   = 0;
    transmitFrameInfo.RemoteFlag = 0;
    transmitFrameInfo.ExternFlag = 0;
    transmitFrameInfo.ID         = ID;
    transmitFrameInfo.DataLen    = 1;
    transmitFrameInfo.Data[0]    = 0x03;
    transmitFrameInfo.Data[1]    = type;
    transmitFrameInfo.Data[2]    = alarm & 0xFF;
    transmitFrameInfo.Data[3]    = alarm >> 8;
    transmitFrameInfo.Data[4]    = base & 0xFF;
    transmitFrameInfo.Data[5]    = base >> 8;
    Transmit(m_DeviceType,0,m_CANIndex,&transmitFrameInfo,1);
}

int CanBusThread::CANStart()
{
    msleep(2000);
    if (StartCAN(m_DeviceType,0,m_CANIndex) == STATUS_OK) {
        qDebug()<<QString("PASS_%1").arg(m_CANIndex)<<" StartCAN OK!";
        return STATUS_OK;
    } else {
        qDebug()<<QString("PASS_%1").arg(m_CANIndex)<<" StartCAN ERROR!";
    }
    return STATUS_ERR;
}

int CanBusThread::CANReset()
{
    msleep(2000);
    if (ResetCAN(m_DeviceType,0,m_CANIndex) == STATUS_OK) {
        qDebug()<<QString("PASS_%1").arg(m_CANIndex)<<" ResetCAN OK!";
        return STATUS_OK;
    } else {
        qDebug()<<QString("PASS_%1").arg(m_CANIndex)<<" ResetCAN ERROR!";
    }
    return STATUS_ERR;
}

CAN_OBJ CanBusThread::confTransmitFrame(int canId)
{
    CAN_OBJ transmitFrameInfo;
    memset(&transmitFrameInfo,0,sizeof(transmitFrameInfo));
    transmitFrameInfo.SendType   = 0;
    transmitFrameInfo.RemoteFlag = 0;
    transmitFrameInfo.ExternFlag = 0;
    transmitFrameInfo.ID         = canId;
    transmitFrameInfo.DataLen    = 1;
    transmitFrameInfo.Data[0]    = 0x01;
    return transmitFrameInfo;
}

CAN_OBJ CanBusThread::confReceiveFrame()
{
    CAN_OBJ receiveFrameInfo;
    memset(&receiveFrameInfo,0,sizeof(receiveFrameInfo));
    return receiveFrameInfo;
}


int CanBusThread::findNodeIndex(int nodeID)
{
    for (int index = 0; index < m_nodeList.count(); index++) {
        if (nodeID == m_nodeList.value(index).first) {
            return index;
        }
    }
    return -1;
}
//#define PRINT
void CanBusThread::canDataParse(int nodeIndex, CAN_OBJ CanFrameInfo)
{
    QList<int> tempList;    tempList.clear();
    QList<int> leakList;    leakList.clear();
    QList<int> earcList;    earcList.clear();
    QList<int> uregList;    uregList.clear();

    int ID    = CanFrameInfo.ID;
    int type  = CanFrameInfo.Data[CAN_TYPE];
    int state = CanFrameInfo.Data[CAN_STATE];

    switch (type) {
    case MOD_LEAK: {
        //实时数据
        int curLeak  = CanFrameInfo.Data[CAN_RD_H]<<8;//高位数
        curLeak |= CanFrameInfo.Data[CAN_RD_L];//低位数
        //报警数值
        int alarmLeak = CanFrameInfo.Data[CAN_AH_BL] >> 4;//取出高4位数
        alarmLeak <<= 8;
        alarmLeak |= CanFrameInfo.Data[CAN_AD_L];//低位数
        //固有漏电
        int baseLeak =  CanFrameInfo.Data[CAN_AH_BL] & 0xF;//取出低4位数
        baseLeak <<= 8;
        baseLeak |= CanFrameInfo.Data[CAN_BD_L];//低位数
        leakList<<nodeIndex<<m_currentPass<<ID<<type<<state<<curLeak<<alarmLeak<<baseLeak;
        emit sigSendCanData(leakList);
    }
        break;
    case MOD_TEMP: {
        int curTemp   = CanFrameInfo.Data[CAN_RD_L];
        int alarmTemp = CanFrameInfo.Data[CAN_AD_L];
        tempList<<nodeIndex<<m_currentPass<<ID<<type<<state<<curTemp<<alarmTemp<<0;
        emit sigSendCanData(tempList);
    }
        break;
    case MOD_EARC:
        earcList<<nodeIndex<<m_currentPass<<ID<<type<<state<<0<<0<<0;
        emit sigSendCanData(earcList);
        break;
    case MOD_UREG:
        uregList<<nodeIndex<<m_currentPass<<ID<<type<<state<<0<<0<<0;
        emit sigSendCanData(uregList);
        break;
    }
}


#define MOD_ALARM   0x01//报警
#define MOD_OFFLINE 0x03//掉线

void CanBusThread::canWorkUnit(int nodeIndex, int nodeID, int nodeType)
{
    msleep(200);
    ClearBuffer(m_DeviceType,0,m_CANIndex);
    CAN_OBJ receiveFrameInfo = confReceiveFrame();
    CAN_OBJ transmitFrameInfo = confTransmitFrame(nodeID);

    int times = 0;
    while (times < TIMES) {
        times++;
        Transmit(m_DeviceType,0,m_CANIndex,&transmitFrameInfo,1);
        msleep(400);
        int recvNum = Receive(m_DeviceType,0,m_CANIndex,&receiveFrameInfo,1,0);
        if (1 == recvNum) {
            if (transmitFrameInfo.ID == receiveFrameInfo.ID) {
                canDataParse(nodeIndex,receiveFrameInfo);//正常数据
                break;

            } else {
                if (receiveFrameInfo.Data[1] == MOD_ALARM) {
                    int pNodeIndex = findNodeIndex(receiveFrameInfo.ID);
                    if (pNodeIndex != -1) {
                        canDataParse(pNodeIndex,receiveFrameInfo);//报警主动上传
                    }
                }
            }

        } else {
            if (TIMES == times) {
                receiveFrameInfo.ID = nodeID;
                receiveFrameInfo.Data[1] = nodeType;
                receiveFrameInfo.Data[2] = MOD_OFFLINE;
                receiveFrameInfo.DataLen = 8;
                canDataParse(nodeIndex,receiveFrameInfo);//通讯故障
            }
        }
    }
}

