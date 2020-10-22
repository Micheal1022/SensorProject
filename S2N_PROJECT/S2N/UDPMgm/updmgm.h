#ifndef BYDZ_UPDMGM_H
#define BYDZ_UPDMGM_H


#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
//#include "bydz_sqlmgm.h"

class UDPMgm : public QObject
{
    Q_OBJECT
public:
    explicit UDPMgm(QObject *parent = NULL);
    ~UDPMgm();
    void initConf(const QHostAddress &host, quint16 port);
    void nodeValue(QList<int> data);

private:
    QUdpSocket *m_udpSocket;
    quint16 m_port;
    QHostAddress m_hostAddr;
    void sendDate(QByteArray byteArray);
    QString getHostIpAddress();
signals:
    void sigReset();
    void sigMute();
private slots:
    void slotReadReady();

};

#endif // BYDZ_UPDMGM_H
