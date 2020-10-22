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
    QByteArray m_recvByteArray;
    QSerialPort *m_serialPort;
    uchar m_mainPower;
    uchar m_backPower;
    uchar m_hostNumber;
    QTimer *m_timer;
    enum NodeSts{
        Normal = 0x0000,
        Alarm  = 0x0001,
        Error  = 0x0008,
        Unmatched = 0x0009//没有节点
    };
    struct nodeInfo
    {
        int state;
        nodeInfo(){
          state = Error;
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
