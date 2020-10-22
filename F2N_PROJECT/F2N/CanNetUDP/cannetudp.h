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
    void confCanNetUDP(quint16 targetPort,quint16 hostPort,QList<int> nodeIDList, int currentPass ,int testTimes);
private:
    QTimer *m_dataTimer;
    int m_testTimes;
    int m_nodeIndex;
    int m_currentPass;
    int m_nodeIDCount;
    QList<int> m_nodeIDList;
    QList<int> m_nodeTimesList;

    QUdpSocket *m_udpSocket;

    quint16 m_targetPort;
    quint16 m_hostPort;
    QHostAddress m_targetIP;
    QHostAddress m_hostIP;
    QByteArray confCanInquiryData(uint canID, uint size);
    void sendDate(QByteArray byteArray);
    void workUnit(int currentNode,uint canID);
    void canDataParse(int currentNode, uint canID, QByteArray recvData);
signals:
    void sigCanBusState(bool state);
    void sinCurCanId(int curCanId);
    void sigSendRS485Data(QList<int> dataList);
    void sigSendCanData(int index,int pass,int canId,int type,int state,QList<int> voltageList,QList<qreal> currentList);
public slots:

private slots:
    void slotReadData();
    void slotTimeOut();
};

#endif // CANNETUDP_H
