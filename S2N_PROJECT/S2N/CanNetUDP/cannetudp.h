#ifndef CANNETUDP_H
#define CANNETUDP_H
#include <QTimer>
#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
class CanNetUDP : public QObject
{
    Q_OBJECT
public:
    explicit CanNetUDP(QObject *parent = 0);
    ~CanNetUDP();
    CanNetUDP(const QHostAddress targetIP, const QHostAddress hostIP);
    void confCanNetUDP(quint16 targetPort, quint16 hostPort, int currentPass , int testTimes);
    void timerPause();
    void timerRestart();
private:
    QTimer *m_dataTimer;
    int m_testTimes;
    int m_nodeIndex;
    int m_currentPass;
    int m_nodeInfoCount;
    QList<int> m_nodeIDList;
    QList<int> m_nodeTimesList;
    QList<QPair<int,int> > m_nodeInfoList;

    QUdpSocket *m_udpSocket;
    bool m_resetFlag;
    quint16 m_targetPort;
    quint16 m_hostPort;
    QHostAddress m_targetIP;
    QHostAddress m_hostIP;
    QByteArray confCanInquiryData(uint canID, uint size);
    QByteArray readData();
    void sendDate(QByteArray byteArray);
    void workUnit(int currentNode, int canID, int nodeType);
    void canDataParse(int nodeIndex, uint canID, QByteArray recvData);

signals:

    void sigThreadPaused(int);
    void sigOffLineTimes(int CANIndex,bool flag);
    void sigSendCanData(QList<int> dataList);

public slots:

private slots:

    void slotTimeOut();
};

#endif // CANNETUDP_H
