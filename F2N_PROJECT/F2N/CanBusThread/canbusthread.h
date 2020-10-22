#ifndef CANBUSTHREAD_H
#define CANBUSTHREAD_H

#include <QMap>
#include <QThread>
#include "ECanVci.h"
#include "GlobalData/globaldata.h"
class CanBusThread : public QThread,public GlobalData
{
    Q_OBJECT
public:
    CanBusThread();
    CanBusThread(int pass,QList<int> nodeListPass);
    ~CanBusThread();
    static DWORD m_devType;
    static bool openCanBusDev();
    static bool closeCanBusDev();
    bool initCanBusDev(unsigned int CANIndex);

private:
    int CANStart();
    int CANReset();
    DWORD m_canPass;
    int m_pass;
    int m_dropTimes;

    int m_currentPass;
    int m_currentNode;
    int m_passNodeCount;
    bool m_passUsable;
    QList<int> m_nodeListPass;
    CAN_OBJ confFrame(uint canId);
    void canDataParse(int currentNode, int devType, int CANInd, CAN_OBJ data);
    void workUnit(int devType, int CANInd, int currentNode, QList<int> nodeList);

signals:
    void sigCanBusState(bool state);
    void sinCurCanId(int curCanId);
    void sigSendRS485Data(QList<int> dataList);
    void sigSendCanData(int index,int pass,int canId,int type,int state,QList<int> voltageList,QList<qreal> currentList);

protected:
    virtual void run() Q_DECL_OVERRIDE;

public slots:
    void slotReset();
};


#endif // CANBUSTHREAD_H
