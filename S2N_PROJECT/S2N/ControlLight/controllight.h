#ifndef CONTROLLIGHT_H
#define CONTROLLIGHT_H

#include <QObject>

class QTimer;
class QSound;
class QSerialPort;

class ControlLight : public QObject
{
    Q_OBJECT
public:
    explicit ControlLight(QObject *parent = 0);
    ~ControlLight();

    void controlReset();
    void setAlarmSound();
    void setErrorSound();
    void setNormalSound();
    bool alarmIsFinished();
    bool errorIsFinished();
    void controlCanLed(bool flag);
    void controlDataTimer(bool flag);
private:

    QTimer *m_dataTimer;
    QSerialPort *m_serialPort;
    QSound *m_errorSound;
    QSound *m_alarmSound;
    int m_step;
    int m_mainPower;
    int m_backPower;
    QByteArray m_recvByteArray;


    QByteArray m_MPNormal;
    QByteArray m_MPError;
    QByteArray m_BPNormal;
    QByteArray m_BPError;
    QByteArray m_alarmLed;
    QByteArray m_errorLed;
    QByteArray m_canLed;
    QByteArray m_muteLed;
    QByteArray m_relayCtrl;
    QByteArray m_allCtrl;
    QByteArray m_powerState;

    void initVariable();
    void initConnect();
    void initSerialDevice();
    bool openSerialDevice();
    void closeSerialDevice();
    QByteArray confSendData(uchar data_1,uchar data_2);
    void sendDataSerial(QByteArray byteArray);


signals:
    void sigReset();
    void sigPowerState(int mainPower,int backPower);
public slots:
    void slotConfSerialData(QList<int> dataList);
private slots:
    void slotDataTimeOut();
    void slotRecvData();
public slots:

    void slotSendDataSerial(uchar data_1,uchar data_2);
    void slotControlSound(int soundType);
};



#endif // CONTROLLIGHT_H
