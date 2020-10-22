#ifndef NODEVIEW_H
#define NODEVIEW_H

#include <QWidget>


#define PASS_1  1
#define PASS_2  2

class QButtonGroup;
class QGridLayout;
class CanBusThread;
class QTimer;
namespace Ui {
class NodeView;
}
class BtnUnitInfo
{
public:

    int m_realLeak;     //实时漏电
    int m_lockLeak;     //漏电报警锁定
    int m_alarmLeak;    //漏电阈值
    int m_baseLeak;     //固有漏电
    int m_realTemp;     //实时温度
    int m_lockTemp;     //温度报警锁定
    int m_alarmTemp;    //温度阈值
    QString m_nodeArea; //模块地址

    int m_nodeID;       //节点ID
    int m_nodeType;     //节点类型
    int m_nodeState;    //节点状态

    bool m_normalFlag;  //正常
    bool m_alarmFlag;   //报警
    bool m_errorFlag;   //故障
    bool m_offLineFlag; //通讯故障

    void initData() {
        m_nodeID     = 0;
        m_nodeType   = 0;//未注册
        m_nodeState  = 0;//节点正常
        m_realLeak   = 0;
        m_lockLeak   = 0;
        m_alarmLeak  = 0;
        m_baseLeak   = 0;
        m_realTemp   = 0;
        m_lockTemp   = 0;
        m_alarmTemp  = 0;

        m_normalFlag = false;
        m_alarmFlag  = false;
        m_errorFlag  = false;
        m_offLineFlag= false;

    }
};
class NodeView : public QWidget
{
    Q_OBJECT

public:
    explicit NodeView(QWidget *parent = 0);
    ~NodeView();
    void nodeReset();
    void nodeConf(int pass, QList<QStringList> stringList);
    void recvCanData(QList<int> canData);

private:
    Ui::NodeView *ui;
    int m_pass;
    int m_curBtn;
    int m_pageCount;
    int m_nodeCount;
    int m_currentPage;
    QTimer *m_timer;
    QButtonGroup *m_btnGroup;
    QGridLayout *m_gridLayout;
    QList<QStringList> m_nodeErrorList;
    QList<QStringList> m_nodeAlarmList;
    QList<BtnUnitInfo> m_tBtnUnitInfoList;

    void initLayout();
    void initConnect();
    void lcdNumberClear();
    void showCurrentNodeValue(int curBtn);
    QString btnSetText(int nodeType);
    void initNodeBtn(QList<QStringList> stringList);
    int getPageCount(int count);

    QStringList addStringList(int pass, int ID, int type, int state, int alarmValue, QString time, QString area);
    void delStringList(QList<QStringList> &stringlist, int pass, int ID, int state);

signals:
    void sigSoundOn();
    void sigThreadRestart(int);
    void sigSetValue(int value);
    void sigInfoCount(int pass,int alarmcount, int errorCount);
private slots:
    void slotTimeOut();
    void slotCurrentBtn(int curBtn);
    void slotBtnHeadPage();//首页
    void slotBtnTailPage(); //尾页
    void slotBtnLastPage(); //上一页
    void slotBtnNestPage(); //下一页
    void slotBtnAlarmInfo();
    void slotBtnErrorInfo();
public slots:


};

#endif // NODEVIEW_H
