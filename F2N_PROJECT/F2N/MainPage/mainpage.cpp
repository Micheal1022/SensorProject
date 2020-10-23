#include "mainpage.h"
#include "ui_mainpage.h"
#include "RS485/rs485.h"
#include "MsgBox/msgbox.h"
#include "NodeInfo/nodeinfo.h"
#include "CanBusThread/canbusthread.h"
#include <SQLite/sqlite.h>
#include <CanNetUDP/cannetudp.h>

#define PASS_1  1
#define PASS_2  2
#define CANUSB  1
#define CANNET  2

MainPage::MainPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainPage)
{
    ui->setupUi(this);
    ui->tBtnUserLogin->setVisible(true);

    ui->tBtnRecordQuery->setEnabled(false);
    ui->tBtnSelfCheck->setEnabled(false);
    ui->tBtnReset->setVisible(false);
    ui->tBtnUserLogout->setVisible(false);
    ui->tBtnNodeManager->setVisible(false);

    ui->tBtnPass1->setVisible(false);
    ui->tBtnPass2->setVisible(false);

    initConnect();
    confNodeInfo();
}

MainPage::~MainPage()
{
    delete ui;
    qDeleteAll(m_threadList.begin(),m_threadList.end());
    m_threadList.clear();
}

void MainPage::confNodeInfo()
{
    qDeleteAll(m_threadList.begin(),m_threadList.end());
    m_threadList.clear();

    qDeleteAll(m_widgetList.begin(), m_widgetList.end());
    m_widgetList.clear();

    while (ui->stackedWgtPass->count()) {
        int index = ui->stackedWgtPass->count() - 1;
        ui->stackedWgtPass->removeWidget(ui->stackedWgtPass->widget(index));
    }
    QSettings settings(QCoreApplication::applicationDirPath()+"/Conf.ini", QSettings::IniFormat);
    //CANNET
    QString pThost  = settings.value("CANNET/THOST_1").toString();
    quint16 pTport_1= settings.value("CANNET/TPORT_1").toInt();
    quint16 pTport_2= settings.value("CANNET/TPORT_2").toInt();
    QString pLhost  = settings.value("CANNET/LHOST_1").toString();
    quint16 pLport_1= settings.value("CANNET/LPORT_1").toInt();
    quint16 pLport_2= settings.value("CANNET/LPORT_2").toInt();
    int     pTimes_1 =settings.value("CANNET/TIMES_1").toInt();
    int     pTimes_2 =settings.value("CANNET/TIMES_2").toInt();
    int     pCantype =settings.value("CANNET/CANTYPE").toInt();
    qDebug("<=======================>");
    qDebug()<<"pThost_1 =====>"<<pThost;
    qDebug()<<"pTport_1 =====>"<<pTport_1;
    qDebug()<<"pTport_2 =====>"<<pTport_2;
    qDebug()<<"pLport_1 =====>"<<pLport_1;
    qDebug()<<"pLport_2 =====>"<<pLport_2;
    qDebug()<<"pTimes_1 =====>"<<pTimes_1;
    qDebug()<<"pTimes_2 =====>"<<pTimes_2;
    qDebug()<<"pCantype =====>"<<pCantype;

    if (CANUSB == pCantype) {
        CanBusThread::openCanBusDev();
    }

    QSqlDatabase db = SQLite::openConnection();
    QList<int> nodeList_1 = SQLite::getNodeList(db,1);
    QList<int> nodeList_2 = SQLite::getNodeList(db,2);
    SQLite::closeConnection(db);

    //通道1
    if(nodeList_1.size() > 0) {
        m_nodeArea_1.clear();
        QSqlDatabase db = SQLite::openConnection();
        for(int i = 0;i < nodeList_1.size();i++) {
            QString passStr  = QString::number(PASS_1);
            QString canIdStr = QString::number(nodeList_1.value(i));
            QString nodeArea = SQLite::getNodeArea(db,passStr,canIdStr);
            m_nodeArea_1.append(nodeArea);
        }
        SQLite::closeConnection(db);

        ui->tBtnPass1->setVisible(true);
        if (CANUSB == pCantype) {
            CanBusThread *canBusThread_1 = new CanBusThread(PASS_1,nodeList_1);
            m_threadList.append(canBusThread_1);
            connect(canBusThread_1, SIGNAL(sigSendCanData(int,int,int,int,int,QList<int>,QList<qreal>)), this, SLOT(slotRecvCanData(int,int,int,int,int,QList<int>,QList<qreal>)));
            connect(canBusThread_1, SIGNAL(sigSendRS485Data(QList<int>)),m_RS485,SLOT(slotNodeState(QList<int>)));
            canBusThread_1->start();

        } else if (CANNET == pCantype) {
            CanNetUDP *pCanNetUDP = new CanNetUDP(QHostAddress(pThost),QHostAddress(pLhost));
            pCanNetUDP->confCanNetUDP(pTport_1,pLport_1,nodeList_1,PASS_1,pTimes_1);
            QThread *thread = new QThread;
            pCanNetUDP->moveToThread(thread);
            thread->start();
            connect(pCanNetUDP, SIGNAL(sigSendCanData(int,int,int,int,int,QList<int>,QList<qreal>)), this, SLOT(slotRecvCanData(int,int,int,int,int,QList<int>,QList<qreal>)));
            connect(pCanNetUDP, SIGNAL(sigSendRS485Data(QList<int>)),m_RS485,SLOT(slotNodeState(QList<int>)));
        }

        m_nodeInfo_1 = new NodeInfo;
        m_nodeInfo_1->nodeConf(PASS_1,nodeList_1);
        ui->stackedWgtPass->addWidget(m_nodeInfo_1);
        connect(m_nodeInfo_1,&NodeInfo::sigErrorCount,this,&MainPage::sigErrorCount_1);
    }

    //通道2
    if(nodeList_2.size() > 0) {
        m_nodeArea_2.clear();
        for(int i = 0;i < nodeList_2.size();i++) {
            QString passStr  = QString::number(PASS_2);
            QString canIdStr = QString::number(nodeList_2.value(i));
            QString nodeArea = SQLite::getNodeArea(db,passStr,canIdStr);
            m_nodeArea_2.append(nodeArea);
        }

        ui->tBtnPass2->setVisible(true);
        if (CANUSB == pCantype) {
            CanBusThread *canBusThread_2 = new CanBusThread(PASS_2,nodeList_2);
            m_threadList.append(canBusThread_2);
            connect(canBusThread_2, SIGNAL(sigSendCanData(int,int,int,int,int,QList<int>,QList<qreal>)), this, SLOT(slotRecvCanData(int,int,int,int,int,QList<int>,QList<qreal>)));
            connect(canBusThread_2, SIGNAL(sigSendRS485Data(QList<int>)),m_RS485,SLOT(slotNodeState(QList<int>)));
            canBusThread_2->start();

        } else if (CANNET == pCantype) {
            CanNetUDP *pCanNetUDP = new CanNetUDP(QHostAddress(pThost),QHostAddress(pLhost));
            pCanNetUDP->confCanNetUDP(pTport_2,pLport_2,nodeList_2,PASS_2,pTimes_2);
            QThread *thread = new QThread;
            pCanNetUDP->moveToThread(thread);
            thread->start();
            connect(pCanNetUDP, SIGNAL(sigSendCanData(int,int,int,int,int,QList<int>,QList<qreal>)), this, SLOT(slotRecvCanData(int,int,int,int,int,QList<int>,QList<qreal>)));
            connect(pCanNetUDP, SIGNAL(sigSendRS485Data(QList<int>)),m_RS485,SLOT(slotNodeState(QList<int>)));
        }

        m_nodeInfo_2 = new NodeInfo;
        m_nodeInfo_2->nodeConf(PASS_2,nodeList_2);
        ui->stackedWgtPass->addWidget(m_nodeInfo_2);
        connect(m_nodeInfo_2,&NodeInfo::sigErrorCount,this,&MainPage::sigErrorCount_2);
    }
}

void MainPage::initConnect()
{
    m_RS485 = new RS485;
    connect(ui->tBtnMute,&QToolButton::clicked,this,&MainPage::slotBtnMute);
    connect(ui->tBtnReset,&QToolButton::clicked,this,&MainPage::slotBtnReset);
    connect(ui->tBtnUserLogout,&QToolButton::clicked,this,&MainPage::slotBtnLogout);
    connect(ui->tBtnUserLogin,&QToolButton::clicked,this,&MainPage::sigBtnUserLogin);
    connect(ui->tBtnRecordQuery,&QToolButton::clicked,this,&MainPage::sigBtnRecordQuery);
    connect(ui->tBtnNodeManager,&QToolButton::clicked,this,&MainPage::sigBtnNodeManager);
    connect(ui->tBtnSelfCheck,&QToolButton::clicked,this,&MainPage::sigBtnSelfCheck);

    connect(ui->tBtnPass1,SIGNAL(clicked(bool)),this,SLOT(slotBtnPass1()));
    connect(ui->tBtnPass2,SIGNAL(clicked(bool)),this,SLOT(slotBtnPass2()));
    connect(m_RS485,SIGNAL(sigMute()),this,SLOT(slotBtnMute()));
    connect(m_RS485,SIGNAL(sigReset()),this,SLOT(slotBtnReset()));
}

void MainPage::slotBtnPass1()
{
    ui->stackedWgtPass->setCurrentIndex(0);
    ui->tBtnPass1->setStyleSheet("QToolButton{font: 14pt '文泉驿等宽微米黑';color: rgb(255, 255, 255);"
                                 "border:1px solid rgb(28, 183, 200);border-top-left-radius:10px;"
                                 "border-top-right-radius:0px;border-bottom-left-radius:0px;"
                                 "border-bottom-right-radius:0px;background-color:rgb(28, 183, 200);}");
    ui->tBtnPass2->setStyleSheet("QToolButton{font: 14pt '文泉驿等宽微米黑';color: rgb(255, 255, 255);"
                                 "border:1px solid rgb(28, 183, 200);border-top-left-radius:0px;"
                                 "border-top-right-radius:10px;border-bottom-left-radius:0px;"
                                 "border-bottom-right-radius:0px;background-color: rgb(15, 41, 68);}");
}

void MainPage::slotBtnPass2()
{
    ui->stackedWgtPass->setCurrentIndex(1);
    ui->tBtnPass1->setStyleSheet("QToolButton{font: 14pt '文泉驿等宽微米黑';color: rgb(255, 255, 255);"
                                 "border:1px solid rgb(28, 183, 200);border-top-left-radius:10px;"
                                 "border-top-right-radius:0px;border-bottom-left-radius:0px;"
                                 "border-bottom-right-radius:0px;background-color: rgb(15, 41, 68);}");
    ui->tBtnPass2->setStyleSheet("QToolButton{font: 14pt '文泉驿等宽微米黑';color: rgb(255, 255, 255);"
                                 "border:1px solid rgb(28, 183, 200);border-top-left-radius:0px;"
                                 "border-top-right-radius:10px;border-bottom-left-radius:0px;"
                                 "border-bottom-right-radius:0px;background-color:rgb(28, 183, 200);}");
}

void MainPage::slotBtnLogout()
{
    ui->tBtnUserLogin->setVisible(true);
    ui->tBtnUserLogout->setVisible(false);
    ui->tBtnNodeManager->setEnabled(false);
    ui->tBtnRecordQuery->setEnabled(false);
    ui->tBtnSelfCheck->setEnabled(false);
}

void MainPage::slotLogin()
{
    ui->tBtnUserLogin->setVisible(false);
    ui->tBtnUserLogout->setVisible(true);
    ui->tBtnNodeManager->setEnabled(true);
    ui->tBtnRecordQuery->setEnabled(true);
    ui->tBtnSelfCheck->setEnabled(true);
}

void MainPage::slotConfNodeInfo()
{
    confNodeInfo();
    slotBtnPass1();
}

void MainPage::slotRecvCanData(int index, int pass, int canId, int type, int state, QList<int> voltageList, QList<qreal> currentList)
{
    if(pass == PASS_1) {
        m_nodeInfo_1->slotRecvCanData(index,pass,canId,type,state,voltageList,currentList);
        emit sigSendCanData(index,pass,canId,type,state,voltageList,currentList,m_nodeArea_1.value(index));//send to UDP
    } else if (pass == PASS_2) {
        m_nodeInfo_2->slotRecvCanData(index,pass,canId,type,state,voltageList,currentList);
        emit sigSendCanData(m_node_1_Count+index,pass,canId,type,state,voltageList,currentList,m_nodeArea_2.value(index));//send to UDP
    }
}

void MainPage::slotBtnMute()
{
    emit sigBtnMute();
    //QSqlDatabase db = SQLite::openConnection();
    //for(int id = 1;id < 139;id++){
    //    SQLite::insertNode(db,QString::number(1),QString::number(id),QString("1"),QString("1"),QString("----"));
    //}
    //for(int id = 1; id < 81;id++){
    //    SQLite::insertNode(db,QString::number(2),QString::number(id),QString("1"),QString("1"),QString("----"));
    //}
    //SQLite::closeConnection(db);
}

void MainPage::slotBtnReset()
{
    int ret = MsgBox::showQuestion(this,tr("操作提示"),tr("你确定要复位所有节点？"),tr("确定"),tr("取消"));
    if(!ret) {
        confNodeInfo();
        slotBtnPass1();
        emit sigBtnReset();
    }
}


