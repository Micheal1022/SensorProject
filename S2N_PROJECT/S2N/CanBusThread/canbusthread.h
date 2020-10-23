#ifndef CANBUSTHREAD_H
#define CANBUSTHREAD_H

#include <QThread>
#include "ECanVci.h"
#define MOD_UREG    0//未注册
#define MOD_LEAK    2//漏电
#define MOD_TEMP    3//温度
#define MOD_EARC    4//电弧
class CanBusThread : public QThread
{
    Q_OBJECT
public:
    CanBusThread();
    ~CanBusThread();
    CanBusThread(DWORD DeviceType,int currentPass);
    DWORD m_DeviceType;
    DWORD m_CANIndex;
    bool initCanBusDev(DWORD DeviceType, DWORD CANIndex);
    void threadPause();
    void threadRestart();
    void controlAlarmData(quint32 ID, quint16 type, quint16 alarm, quint16 base = 0);
signals:
    void sigThreadPaused(int);
    void sigOffLineTimes(int CANIndex,bool flag);
    void sigSendCanData(QList<int> dataList);
private:
    DWORD m_canPass;
    int m_currentPass;
    bool m_reset;
    int m_nodeIndex;
    int m_nodeCount;
    bool m_offLineFlag;
    int m_offLineTimes;
    QList<QPair<int,int> > m_nodeList;
private:
    int CANStart();
    int CANReset();
    CAN_OBJ confTransmitFrame(int canId);
    CAN_OBJ confReceiveFrame();

    int findNodeIndex(int nodeID);
    void canDataParse(int nodeIndex, CAN_OBJ CanFrameInfo);
    void canWorkUnit(int nodeIndex, int nodeID, int nodeType);
protected:
    virtual void run() Q_DECL_OVERRIDE;
};

#endif // CANBUSTHREAD_H
