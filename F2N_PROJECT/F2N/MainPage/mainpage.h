#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>

namespace Ui {
class MainPage;
}
class RS485;
class NodeInfo;
class MainPage : public QWidget
{
    Q_OBJECT

public:
    explicit MainPage(QWidget *parent = 0);
    ~MainPage();
    void confNodeInfo();

private:
    Ui::MainPage *ui;
    NodeInfo *m_nodeInfo_1;
    NodeInfo *m_nodeInfo_2;

    int m_node_1_Count;
    int m_node_2_Count;
    RS485 *m_RS485;
    QList<QString> m_nodeArea_1;
    QList<QString> m_nodeArea_2;
    QList<QWidget *> m_widgetList;
    QList<QThread *> m_threadList;
    void initConnect();
signals:
    void sigBtnMute();
    void sigBtnReset();
    void sigBtnNodeInfo();
    void sigBtnSelfCheck();
    void sigBtnNodeManager();
    void sigBtnRecordQuery();
    void sigBtnUserLogin();
    void sigErrorCount_1(int errorNum);
    void sigErrorCount_2(int errorNum);
    void sigSendCanData(int index,int pass,int canId,int type,int state,QList<int> voltageList,QList<qreal> currentList,QString area);

private slots:
    void slotBtnPass1();
    void slotBtnPass2();
    void slotBtnLogout();

public slots:
    void slotLogin();
    void slotBtnMute();
    void slotBtnReset();
    void slotConfNodeInfo();
    void slotRecvCanData(int index,int pass,int canId,int type,int state,QList<int> voltageList,QList<qreal> currentList);




};

#endif // MAINPAGE_H
