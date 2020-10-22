#ifndef NODEINFO_H
#define NODEINFO_H

#include <QWidget>
class QButtonGroup;
class QGridLayout;
class CanBusThread;
class QTimer;
namespace Ui {
class NodeInfo;
}
class BtnUnitInfo
{
public:

    /*
     *通讯故障>供电中断>缺相>欠压>过压>过流
     */
    QString m_valueAV_1; //
    QString m_valueBV_1; //
    QString m_valueCV_1; //
    QString m_valueAI_1; //
    QString m_valueBI_1; //
    QString m_valueCI_1; //
    QString m_valueAV_2; //
    QString m_valueBV_2; //
    QString m_valueCV_2; //
    QString m_nodeArea; //模块地址

    int m_canId;
    int m_nodeState;
    int m_nodeType;
    int m_ratio;        //变比
    bool m_nodeNormal;  //正常
    bool m_overCurrent; //过流
    bool m_lackPhase;   //错相
    bool m_overVoltage; //过压
    bool m_underVoltage;//欠压
    bool m_powerLost;   //供电中断
    bool m_canBusError; //通讯中断
    void initData() {
        m_canId = 0;
        m_nodeType  = 0;
        m_nodeState = 0;
        m_valueAV_1 = "0";
        m_valueBV_1 = "0";
        m_valueCV_1 = "0";
        m_valueAI_1 = "0";
        m_valueBI_1 = "0";
        m_valueCI_1 = "0";
        m_valueAV_2 = "0";
        m_valueBV_2 = "0";
        m_valueCV_2 = "0";
        m_overCurrent = false;
        m_lackPhase   = false;
        m_overVoltage = false;
        m_underVoltage= false;
        m_powerLost   = false;
        m_canBusError = false;
        m_nodeArea  = "-";
    }
};
class NodeInfo : public QWidget
{
    Q_OBJECT

public:
    explicit NodeInfo(QWidget *parent = 0);
    ~NodeInfo();
    void nodeConf(int pass, QList<int> nodeList);

private:
    Ui::NodeInfo *ui;
    int m_pass;
    int m_curBtn;
    int m_pageCount;
    int m_nodeCount;
    int m_currentPage;
    QTimer *m_timer;
    QList<QStringList> m_nodeErrorList;
    QList<QStringList> m_powerLostList;
    QButtonGroup *m_tBtnGroup;
    QGridLayout *m_gridLayout;
    QList<BtnUnitInfo> m_tBtnUnitInfoList;

    void initLayout();
    void lcdNumberClear();
    void showCurNodeValue(int curBtn);
    void initNodeBtn(QList<int> nodeList);
    int getPageCount(QList<int> nodeList);
    QStringList addStringList(int pass, int canId, int state, QString alarmTime, QString area);
    void delStringList(QList<QStringList> &stringList, int pass, int canId, int state);
    void initConnect();
    QString btnSetText(int nodeType);
    void nodeValueType(int nodeType);
    void updateNodeState(int index, int canId);

signals:
    void sigSoundOn();
    void sigSetValue(int);
    void sigErrorCount(int errorNum);
    void sigSendCanData(int index,int pass,int canId,int type,int state,QList<int> voltageList,QList<qreal> currentList);
private slots:
    void slotBtnPowerInfo();
    void slotBtnErrorInfo();
    void slotBtnGroup(int curBtn);
    void slotTimeOut();

    void slotBtnHeadPage();//首页
    void slotBtnTailPage(); //尾页
    void slotBtnLastPage(); //上一页
    void slotBtnNestPage(); //下一页
public slots:
    void slotCurCanId(int curCanId);
    void slotRecvCanData(int index,int pass,int canId,int type,int state,QList<int> voltageList,QList<qreal> currentList);

};

#endif // NODEINFO_H
