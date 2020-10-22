#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <GlobalData/globaldata.h>
namespace Ui {
class MainWindow;
}
class QTimer;
class MainPage;
class SelfCheck;
class IOManager;
class UDPServer;
class UserLogin;
class NodeManager;
class RecordQuery;
class MainWindow : public QMainWindow,public GlobalData
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    MainPage  *m_mainPage;
    UDPServer *m_udpServer;
    SelfCheck *m_selfCheck;
    UserLogin *m_userLogin;
    IOManager *m_IOManager;
    NodeManager *m_nodeManager;
    RecordQuery *m_recordQuery;
    //print
    QString m_printName;
    //RS485
    QString m_RS485Name;
    bool m_RS485Enable;
    //UDP
    bool m_UDPEnable_1;
    QString m_host_1;
    quint16 m_port_1;
    //IO
    QString m_IOName;

    QTimer *m_systemTime;

    QTimer *m_IOTimer;
    int  m_errorCount;
    int  m_errorNum_1;
    int  m_errorNum_2;

    int  m_step;
    int  m_oldErrorCount;
    uint m_errorOnTimes;
    uint m_errorOffTimes;
    bool m_mainPowerState;
    bool m_mainPowerFlag;
    bool m_mainErrorFlag;
    uint m_mainPowerTimes;
    uint m_mainErrorTimes;
    bool m_backPowerState;
    bool m_backPowerFlag;
    bool m_backErrorFlag;
    uint m_backPowerTimes;
    uint m_backErrorTimes;

    bool m_canState_1;
    bool m_canState_2;
    uint m_canOnTimes;
    uint m_canOffTimes;

    bool m_muteState;
    uint m_muteOnTimes;
    uint m_muteOffTimes;
    bool m_selfCheckFlag;



private:
    void initVar();
    void initConnect();
    void readConfFile();

    void controlRelay();
    void controlCanLight();
    void controlMuteLight();
    void controlPowerLost();
    void controlErrorLight();
    void controlMainPowerLight();
    void controlBackPowerLight();


signals:
    void sigControlLight(char type,char control);
    void sigControlSound(int type);
private slots:
    void slotBtnMute();
    void slotBtnNodeInfo();
    void slotBtnNodeManager();
    void slotBtnRecordQuery();
    void slotBtnUserLogin();
    void slotBtnSelfCheck();
    void slotSelfCheckFinish();

    void slotReset();
    void slotSystemTime();

    void slotIOTimeOut();
    void slotPowerState(int mainPower,int backPower);
    void slotErrorCount_1(int errorNum);
    void slotErrorCount_2(int errorNum);
    void slotCanBusState_1(bool canState);
    void slotCanBusState_2(bool canState);



};

#endif // MAINWINDOW_H
