#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>
namespace Ui {
class MainPage;
}
class RS485;
class QTimer;
class UDPMgm;
class NodeView;
class CanNetUDP;
class SelfCheck;
class CanBusThread;
class ControlLight;
class MainPage : public QWidget
{
    Q_OBJECT

public:
    explicit MainPage(QWidget *parent = 0);
    ~MainPage();

    void confUserType(int userType);
private:

    Ui::MainPage *ui;
    QTimer *m_timer;
    int m_times;
    bool m_muteFlag;
    int m_userType;
    bool m_selfCheckFlag;
    bool m_udpFlag_1;
    bool m_udpFlag_2;
    bool m_udpFlag_3;
    ControlLight *m_controlLight;
    int m_errorCount;
    int m_alarmCount;
    int m_pass1Count;
    int m_pass2Count;
    QList<int> m_alarmCountList;
    QList<int> m_errorCountList;
    bool m_mainPower;
    bool m_backPower;
    bool m_mainPowerFlag;
    bool m_backPowerFlag;
    QString m_hostArea;
    RS485 *m_RS485;
    UDPMgm *m_udpMgm_1;
    UDPMgm *m_udpMgm_2;
    UDPMgm *m_udpMgm_3;
    SelfCheck *m_selfCheck;
    NodeView *m_nodeView_1;
    NodeView *m_nodeView_2;
    bool can_1_flag;
    bool can_2_flag;
    CanBusThread *m_canBusThread_1;
    CanBusThread *m_canBusThread_2;
    int m_canType;
    CanNetUDP *m_canNetUDP_1;
    CanNetUDP *m_canNetUDP_2;
    void insertList();
    void initConnect();
    void initVariable();
    void confNodeInfo();
    void controlCanLed();
    void controlSoundLed();

    void replaceList(int pass);
    void setErrorCount(int pass,int errorCount);
    void setAlarmCount(int pass,int alarmCount);
signals:
    void sigBtnMute();          //静音
    void sigBtnReset();         //复位
    void sigBtnRecord();   //查询
    void sigBtnLogin();     //登录
    void sigBtnLogout();    //退出
    void sigBtnNodeInfo();  //节点配置

    void sigConfSerialData(QList<int>);

private slots:

    void slotTimeOut();
    void slotOffLine(int CANIndex,bool flag);
    void slotRecvCanData(QList<int> canData);
    void slotSendUdpData(QList<int> canData);
    void slotBtnPass1();
    void slotBtnPass2();

    void slotBtnMute();
    void slotBtnLogout();
    void slotBtnReset();
    void slotUdpReset();
    void slotNodeInfoCount(int pass, int errorCount,int alarmCount);
    void slotBtnSelfCheck();     //自检
    void slotThreadPaused(int CANIndex);
    void slotThreadRestart(int CANIndex);

    void slotPowerState(int mainPower, int backPower);
    void slotSendAlarmData(int pass, quint32 ID, quint16 type, quint16 alarm, quint16 base);

};

#endif // MAINPAGE_H
