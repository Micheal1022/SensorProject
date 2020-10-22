#ifndef RS485_H
#define RS485_H

#include <QObject>
class QTimer;
class QSerialPort;
class RS485 : public QObject
{
    Q_OBJECT
public:
    explicit RS485(QObject *parent = NULL);
    ~RS485();

private:
    void initProt();
    void initConnect();
    quint16 dataCRC16(uchar *data, int len);
    void sendData(char code, qint16 reg, qint16 date);
    void sendData(char code, char* data, int len);

private:
    QByteArray m_tempByteArray;
    QByteArray m_recvByteArray;
    QSerialPort *m_serialPort;
    QTimer *m_timer;
    uchar m_mainPower;
    uchar m_backPower;
    uchar m_hostNumber;

    enum PowerSts{
        Normal,
    };

    enum NodeSts{
        NodeNormal      = 0x00,//正常
        NodeOverCurrent = 0x01,//过流
        NodeLackPhase   = 0x02,//缺相
        NodeOverVoltage = 0x04,//过压
        NodeUnderVoltage= 0x08,//欠压
        NodePowerLost   = 0x10,//供电中断
        NodeCanBusError = 0x20 //通讯中断
    };
    struct nodeInfo {
        int state;
        nodeInfo(){
          state = NodeNormal;
        }
    };
    nodeInfo m_nodeArray[2048];

signals:
    void sigMute();
    void sigReset();
private slots:
    void slotRecvAll();
    void slotTimeOut();

public slots:
    void slotNodeState(QList<int> dataList);
    void slotPowerState(uchar mainPower, uchar backPower);
};

#endif // RS485BUS_H
