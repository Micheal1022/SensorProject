#ifndef IOMANAGER_H
#define IOMANAGER_H

#include <QObject>

const char powerNormal = 0x01;
const char bpowerNormal= 0x02;
const char powerError  = 0x03;
const char bpowerError = 0x04;
const char alarmLight  = 0x05;
const char errorLight  = 0x06;
const char canError    = 0x07;
const char muteLight   = 0x08;
const char relayCont   = 0x09;
const char allLight    = 0xFF;

const char on  = 1;
const char off = 0;


class QSound;
class QSerialPort;
class IOManager : public QObject
{
    Q_OBJECT
public:
    explicit IOManager(QObject *parent = NULL);
    IOManager(QString IOName);
    ~IOManager();
private:
    QSound *errorSound;
    QSerialPort *m_serialPort;
    QByteArray m_byteArray;

    void initSerialPort(QString IOName);
signals:
    void sigPowerState(int mainPower,int backPower);
    void sigResetSwitch();
private slots:
    void slotReadyRead();
public slots:
    void slotControlSound(int type);
    void slotControlLight(char type, char control);

};

#endif // IOMANAGER_H
