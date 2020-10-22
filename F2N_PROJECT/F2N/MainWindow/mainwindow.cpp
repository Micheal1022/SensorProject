#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <MainPage/mainpage.h>
#include "NodeInfo/nodeinfo.h"
#include "UserLogin/userlogin.h"
#include <SQLite/sqlite.h>
#include <QSettings>
#include <QSqlDatabase>
#include <SelfCheck/selfcheck.h>
#include <NodeManager/nodemanager.h>
#include <RecordQuery/recordquery.h>
#include <IOManager/iomanager.h>
#include <UDPServer/udpserver.h>
#include "CanNetUDP/cannetudp.h"



//#define FULL
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
#ifdef FULL
    setWindowFlags(Qt::FramelessWindowHint);
    showFullScreen();
#endif
    initVar();
    readConfFile();
    m_mainPage = new MainPage;
    ui->stackedWidget->addWidget(m_mainPage);
    m_nodeManager = new NodeManager;
    ui->stackedWidget->addWidget(m_nodeManager);
    m_recordQuery = new RecordQuery;
    ui->stackedWidget->addWidget(m_recordQuery);
    m_userLogin   = new UserLogin;
    ui->stackedWidget->addWidget(m_userLogin);

    m_IOManager = new IOManager(m_IOName);
    m_selfCheck = new SelfCheck;
    if (m_UDPEnable_1 == true)
    {
        m_udpServer = new UDPServer;
        m_udpServer->initConf(QHostAddress(m_host_1),(quint16)m_port_1);
        //通讯收发
        //静音
        //connect(bydzUdpMgm_1,&BYDZ_UDPMgm::sigMute,prompt,&Prompt::slotMuteState);
        //connect(bydzUdpMgm_1,&BYDZ_UDPMgm::sigReset,bydzCan,&BYDZ_CanMgm::slotCanReset);
        connect(m_mainPage,SIGNAL(sigSendCanData(int,int,int,int,int,QList<int>,QList<qreal>,QString)),
                m_udpServer,SLOT(slotSendCanData(int,int,int,int,int,QList<int>,QList<qreal>,QString)));
    }



    initConnect();



#ifdef BEVONE
    ui->lbLogo->setStyleSheet("border-image: url(:/Image/Bevone.png);");
    ui->lbCompany->setText(tr("北京北元安达电子有限公司"));
#else
    ui->lbLogo->setStyleSheet("border-image: url(:/Image/Sensor.png);");
    ui->lbCompany->setText(tr("西安盛赛尔电子有限公司"));
#endif


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initVar()
{
    m_errorCount = 0;
    m_errorNum_1 = 0;
    m_errorNum_2 = 0;

    m_step = 0;
    m_oldErrorCount = 0;
    m_errorOnTimes = 0;
    m_errorOffTimes = 0;
    m_mainPowerState = false;
    m_mainPowerFlag = false;
    m_mainErrorFlag = false;
    m_mainPowerTimes = 0;
    m_mainErrorTimes = 0;
    m_backPowerState = false;
    m_backPowerFlag = false;
    m_backErrorFlag = false;
    m_backPowerTimes = 0;
    m_backErrorTimes = 0;

    m_canState_1 = true;
    m_canState_2 = true;
    m_canOnTimes = 0;
    m_canOffTimes = 0;

    m_muteState = false;
    m_muteOnTimes = 0;
    m_muteOffTimes= 0;
    m_selfCheckFlag = false;
}

void MainWindow::initConnect()
{

    connect(m_userLogin,&UserLogin::sigLogin,m_mainPage,&MainPage::slotLogin);
    connect(m_userLogin,&UserLogin::sigNodeInfo,this,&MainWindow::slotBtnNodeInfo);
    connect(m_recordQuery,&RecordQuery::sigNodeInfo,this,&MainWindow::slotBtnNodeInfo);
    connect(m_nodeManager,&NodeManager::sigNodeInfo,this,&MainWindow::slotBtnNodeInfo);

    connect(m_mainPage,&MainPage::sigBtnMute,this,&MainWindow::slotBtnMute);
    connect(m_mainPage,&MainPage::sigBtnReset,this,&MainWindow::slotReset);
    connect(m_mainPage,&MainPage::sigBtnSelfCheck,this,&MainWindow::slotBtnSelfCheck);
    connect(m_mainPage,&MainPage::sigBtnUserLogin,this,&MainWindow::slotBtnUserLogin);
    connect(m_mainPage,&MainPage::sigBtnRecordQuery,this,&MainWindow::slotBtnRecordQuery);
    connect(m_mainPage,&MainPage::sigBtnNodeManager,this,&MainWindow::slotBtnNodeManager);
    connect(m_mainPage,&MainPage::sigErrorCount_1,this,&MainWindow::slotErrorCount_1);
    connect(m_mainPage,&MainPage::sigErrorCount_2,this,&MainWindow::slotErrorCount_2);
    connect(this,&MainWindow::sigControlLight,m_IOManager,&IOManager::slotControlLight);
    connect(this,&MainWindow::sigControlSound,m_IOManager,&IOManager::slotControlSound);
    connect(m_nodeManager,&NodeManager::sigSaveOk,m_mainPage,&MainPage::slotConfNodeInfo);
    connect(m_selfCheck,&SelfCheck::sigSelfCheckFinish,this,&MainWindow::slotSelfCheckFinish);
    connect(m_selfCheck,&SelfCheck::sigControlLight,m_IOManager,&IOManager::slotControlLight);
    connect(m_selfCheck,&SelfCheck::sigControlSound,m_IOManager,&IOManager::slotControlSound);

    connect(m_IOManager,&IOManager::sigPowerState,this,&MainWindow::slotPowerState);

    m_IOTimer = new QTimer;
    connect(m_IOTimer,&QTimer::timeout,this,&MainWindow::slotIOTimeOut);
    m_IOTimer->start(1000);

    m_systemTime = new QTimer;
    connect(m_systemTime,&QTimer::timeout,this,&MainWindow::slotSystemTime);
    m_systemTime->start(1000);
}

void MainWindow::readConfFile()
{
    QSettings settings(QCoreApplication::applicationDirPath()+"/Conf.ini", QSettings::IniFormat);
    //print
    m_printName = settings.value("PRINT/PrintName").toString();
    //io
    m_IOName = settings.value("LIGHT/LightName").toString();
    //485
    m_RS485Name   = settings.value("RS485/RS485Name").toString();
    m_RS485Enable = settings.value("RS485/RS485Enable").toBool();
    //udp
    m_UDPEnable_1 = settings.value("NETWORK_1/UDPEnable").toBool();
    m_host_1      = settings.value("NETWORK_1/HOST").toString();
    m_port_1      = settings.value("NETWORK_1/PORT").toInt();

}

void MainWindow::controlRelay()
{
    if (m_errorCount > 0) {
        m_step = 2;
        emit sigControlLight(relayCont,on);
    } else {
        m_step = 2;
        emit sigControlLight(relayCont,off);
    }
}

void MainWindow::controlCanLight()
{
    if(m_canState_1 == false || m_canState_2 == false) {
        m_canOnTimes++;
        if(m_canOnTimes == 1) {
            m_step = 6;
            m_canOnTimes = 0;
            m_canOffTimes = 0;
            emit sigControlLight(canError,off);
        }
    }
    else
    {
        m_canOffTimes++;
        if(m_canOffTimes == 1)
        {
            m_step = 6;
            m_canOnTimes = 0;
            m_canOffTimes = 0;
            emit sigControlLight(canError,on);
        }
    }
}

void MainWindow::controlMuteLight()
{
    if(m_muteState == true)
    {
        m_muteOnTimes++;
        if(m_muteOnTimes == 1)
        {
            m_step = 0;
            m_muteOnTimes = 0;
            m_muteOffTimes = 0;
            emit sigControlLight(muteLight,on);
        }
    }
    else
    {
        m_muteOffTimes++;
        if(m_muteOffTimes == 1)
        {
            m_step = 0;
            m_muteOnTimes = 0;
            m_muteOffTimes = 0;
            emit sigControlLight(muteLight,off);
        }
    }
}

void MainWindow::controlPowerLost()
{
    m_step = 1;
}

void MainWindow::controlErrorLight()
{
    if(m_errorCount > 0 || m_mainPowerState == false || m_backPowerState == false || m_canState_1 == false || m_canState_2 == false)
    {
        m_errorOnTimes++;
        if(m_errorOnTimes == 1)
        {
            m_step = 5;
            m_errorOnTimes = 0;
            m_errorOffTimes = 0;
            emit sigControlLight(errorLight,on);
        }
    }
    else
    {
        m_errorOffTimes++;
        if(m_errorOffTimes == 1)
        {
            m_step = 5;
            m_errorOnTimes = 0;
            m_errorOffTimes = 0;
            emit sigControlLight(errorLight,off);
        }
    }
}

void MainWindow::controlMainPowerLight()
{
    //主电正常
    if (m_mainPowerState == true) {
        m_mainPowerTimes++;
        if(m_mainPowerTimes == 1) {
            emit sigControlLight(powerError,off);
        } else if(m_mainPowerTimes == 2){
            m_step = 3;
            m_mainPowerTimes = 0;
            emit sigControlLight(powerNormal,on);
            if (m_mainErrorFlag == false) {
                m_mainPowerFlag = false;
                m_mainErrorFlag = true;
            }
        }
    } else {
        m_mainErrorTimes++;
        if (m_mainErrorTimes == 1) {
            emit sigControlLight(powerError,on);
        } else if (m_mainErrorTimes == 2) {
            m_step = 3;
            m_mainErrorTimes = 0;
            emit sigControlLight(powerNormal,off);

            if (m_mainPowerFlag == false) {
                m_muteState = false;
                m_mainPowerFlag = true;
                m_mainErrorFlag = false;
                QSqlDatabase db = SQLite::openConnection();
                SQLite::insertRecord(db,0,0,7,QDateTime::currentDateTime().toTime_t(),tr("    "));
                SQLite::closeConnection(db);
            }
        }
    }
}

void MainWindow::controlBackPowerLight()
{
    //备电正常
    if (m_backPowerState == true) {

        m_backPowerTimes++;
        if (m_backPowerTimes == 1) {
            emit sigControlLight(bpowerError,off);
        }
        else if (m_backPowerTimes == 2) {
            m_step = 4;
            m_backPowerTimes = 0;
            emit sigControlLight(bpowerNormal,on);
            if (m_backPowerFlag == false) {
                m_backErrorFlag = false;
                m_backPowerFlag = true;
            }
        }
    } else {
        m_backErrorTimes++;
        if (m_backErrorTimes == 1) {
            emit sigControlLight(bpowerNormal,off);
        } else if (m_backErrorTimes == 2) {
            m_step = 4;
            m_backErrorTimes = 0;
            emit sigControlLight(bpowerError,on);
            if (m_backErrorFlag == false) {
                m_muteState = false;
                m_backErrorFlag = true;
                m_backPowerFlag = false;
                QSqlDatabase db = SQLite::openConnection();
                SQLite::insertRecord(db,0,0,8,QDateTime::currentDateTime().toTime_t(),tr("    "));
                SQLite::closeConnection(db);
            }
        }
    }
}

void MainWindow::slotBtnMute()
{
    m_muteState = true;
}


void MainWindow::slotBtnNodeInfo()
{
    ui->stackedWidget->setCurrentIndex(NodeInfoType);
}

void MainWindow::slotBtnNodeManager()
{
    ui->stackedWidget->setCurrentIndex(NodeManagerType);
    m_nodeManager->confShow();
}

void MainWindow::slotBtnRecordQuery()
{
    m_recordQuery->confShow(Admin);
    ui->stackedWidget->setCurrentIndex(RecordQueryType);
}

void MainWindow::slotBtnUserLogin()
{
    m_userLogin->confShow();
    ui->stackedWidget->setCurrentIndex(UserLoginType);
}

void MainWindow::slotBtnSelfCheck()
{
    m_IOTimer->stop();
    m_selfCheck->selfCheckStart();
}

void MainWindow::slotSelfCheckFinish()
{
    m_IOTimer->start();
}

void MainWindow::slotReset()
{
    initVar();
}

void MainWindow::slotSystemTime()
{
    ui->lbTime->setText(QDateTime::currentDateTime().toString("yyyy年MM月dd日 hh:mm:ss"));
}

void MainWindow::slotIOTimeOut()
{
    m_errorCount = m_errorNum_1 + m_errorNum_2;
    //控制声音
    if(m_errorCount != m_oldErrorCount)
    {
        if(m_errorCount > m_oldErrorCount)
        {
            m_muteState = false;
        }
        m_oldErrorCount = m_errorCount;
    }

    if(m_muteState == false)
    {
        if(m_oldErrorCount > 0 || m_mainPowerState == false || m_backPowerState == false)
        {
            emit sigControlSound(ErrorSound);
        }
        else
        {
            emit sigControlSound(NormalSound);
        }
    }
    else
    {
        emit sigControlSound(NormalSound);
    }

    //    qDebug("*************************");
    //    qDebug()<<"m_errorNum_1    : "<<m_errorNum_1;
    //    qDebug()<<"m_errorNum_2    : "<<m_errorNum_2;
    //    qDebug()<<"m_errorCount    : "<<m_errorCount;
    //    qDebug()<<"m_oldErrorCount : "<<m_oldErrorCount;
    //    qDebug()<<"m_muteState     : "<<m_muteState;
    //    qDebug()<<"m_mainPowerState: "<<m_mainPowerState;
    //    qDebug()<<"m_backPowerState: "<<m_backPowerState;

    //控制指示灯
    switch (m_step) {
    case 0://电源中断
        controlPowerLost();
        break;
    case 1://继电器
        controlRelay();
        break;
    case 2://主电
        controlMainPowerLight();
        break;
    case 3://备电
        controlBackPowerLight();
        break;
    case 4://故障
        controlErrorLight();
        break;
    case 5://通讯指示灯x1
        controlCanLight();
        break;
    case 6://静音指示灯
        controlMuteLight();
        break;
    }

}

void MainWindow::slotPowerState(int mainPower, int backPower)
{
    if (mainPower) {
        m_mainPowerState = true;
    } else {
        m_mainPowerState = false;
    }

    if (backPower) {
        m_backPowerState = true;
    } else{
        m_backPowerState = false;
    }
}

void MainWindow::slotErrorCount_1(int errorNum)
{
    m_errorNum_1 = errorNum;

}

void MainWindow::slotErrorCount_2(int errorNum)
{
    m_errorNum_2 = errorNum;

}

void MainWindow::slotCanBusState_1(bool canState)
{
    m_canState_1 = canState;
}

void MainWindow::slotCanBusState_2(bool canState)
{
    m_canState_2 = canState;
}
