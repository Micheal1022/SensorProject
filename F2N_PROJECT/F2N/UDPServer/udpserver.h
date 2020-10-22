#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
class UDPServer : public QObject
{
    Q_OBJECT
public:
    explicit UDPServer(QObject *parent = NULL);
    ~UDPServer();
    void initConf(const QHostAddress targetHost, quint16 targetPort);
private:
    quint16 m_targetPort;
    QUdpSocket *m_udpSocket;
    QHostAddress m_targetHost;
    QString getHostIPAddr();
    void sendDate(QByteArray byteArray);

signals:
    void sigReset();
    void sigMute();
private slots:
    void slotReadReady();

public slots:
    void slotSendCanData(int index, int pass, int canId, int type, int state, QList<int> voltageList, QList<qreal> currentList, QString area);
};

#endif // UDPSERVER_H
