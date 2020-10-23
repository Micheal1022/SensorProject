#include "mainpage.h"
#include "ui_mainpage.h"
#include <QDebug>

#include "RS485/rs485.h"
#include "SQLite/sqlite.h"
#include "MsgBox/msgbox.h"
#include "NodeView/nodeview.h"
#include "SelfCheck/selfcheck.h"
#include "CanBusThread/candevice.h"
#include "CanBusThread/canbusthread.h"
#include <ControlLight/controllight.h>
#include <UDPMgm/updmgm.h>
#include "CanNetUDP/cannetudp.h"

#define USER    0
#define ADMIN   1
#define SUPER   2

#define CANUSB  1
#define CANNET  2


//#define PRINT

MainPage::MainPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainPage)
{
    ui->setupUi(this);
    initVariable();
    confNodeInfo();
    initConnect();

}

MainPage::~MainPage()
{
    CanDevice::closeCanBusDev(USBCAN);
    delete m_nodeView_1;
    delete m_nodeView_2;
    delete ui;
}

/*替换链表数据*/
void MainPage::replaceList(int pass)
{
    m_alarmCountList.replace(pass,0);
    m_errorCountList.replace(pass,0);

}
/*插入链表数据*/
void MainPage::insertList()
{
    for (int index = 0; index < 2;index++) {
        m_alarmCountList.insert(index,0);
        m_errorCountList.insert(index,0);
    }
}

void MainPage::controlSoundLed()
{
    int muteLed  = 0;
    int errorLed = 0;
    int alarmLed = 0;
    QList<int> ledList;
    //主电信息
    if (false == m_mainPower) {
        if (false == m_mainPowerFlag) {
            //关闭静音指示灯
            muteLed = 0;
            m_muteFlag = false;
            m_mainPowerFlag = true;
            QSqlDatabase db = SQLite::openConnection();
            SQLite::insertRecord(db,0,0,1,4,0,QDateTime::currentDateTime().toTime_t(),m_hostArea);
            SQLite::closeConnection(db);
        }
    } else if (true == m_mainPower){
        m_mainPowerFlag = false;
    }
    //备电信息
    if (false == m_backPower) {
        if (false == m_backPowerFlag) {
            //关闭静音指示灯
            muteLed = 0;
            m_muteFlag = false;
            m_backPowerFlag = true;
            QSqlDatabase db = SQLite::openConnection();
            SQLite::insertRecord(db,0,0,1,5,0,QDateTime::currentDateTime().toTime_t(),m_hostArea);
            SQLite::closeConnection(db);
        }
    } else if (true == m_backPower){
        m_backPowerFlag = false;
    }

    /*指示灯控制*/
    if (0 == m_alarmCount) {
        alarmLed = 0;//报警指示灯亮
    } else {
        alarmLed = 1;//报警指示灯灭
    }

    if (false == m_mainPower || false == m_backPower || 0 != m_errorCount) {
        errorLed = 1;//故障指示灯常亮
    } else {
        errorLed = 0;//故障指示灯关闭
    }

    /*声音控制*/
    if (0 == m_alarmCount) {
        if (false == m_mainPower || false == m_backPower || 0 != m_errorCount) {
            if (true == m_muteFlag) {
                muteLed = 1;
                m_controlLight->setNormalSound();//故障声音关闭
            } else {
                muteLed = 0;
                m_controlLight->setErrorSound();//故障声音开启
            }
        } else {//不存在故障
            m_controlLight->setNormalSound();//声音关闭
        }
    } else {
        if (false == m_mainPower || false == m_backPower || 0 != m_errorCount) {
            if (true == m_muteFlag) {
                muteLed = 1;
                m_controlLight->setNormalSound();//声音关闭
            } else {
                muteLed = 0;
                m_controlLight->setAlarmSound();//报警声音开启
            }
        } else {
            if (true == m_muteFlag) {
                muteLed = 1;
                m_controlLight->setNormalSound();//声音关闭
            } else {
                muteLed = 0;
                m_controlLight->setAlarmSound();//报警声音开启
            }
        }
    }

    ledList<<errorLed<<alarmLed<<muteLed;
    emit sigConfSerialData(ledList);
}

void MainPage::slotSendUdpData(QList<int> canData)
{
    if (true == m_udpFlag_1) {
        if (PASS_1 == canData.value(1)) {
            m_udpMgm_1->nodeValue(canData);
        }
        if (PASS_2 == canData.value(1)) {
            QList<int> canTempData = canData;
            canTempData[0] = canData.value(0) + m_pass1Count;
            m_udpMgm_1->nodeValue(canTempData);
        }
    }

    if (true == m_udpFlag_2) {
        if (PASS_1 == canData.value(1)) {
            m_udpMgm_2->nodeValue(canData);
        }
        if (PASS_2 == canData.value(1)) {
            QList<int> canTempData = canData;
            canTempData[0] = canData.value(0) + m_pass1Count;
            m_udpMgm_2->nodeValue(canTempData);
        }
    }

    if (true == m_udpFlag_3) {
        if (PASS_1 == canData.value(1)) {
            m_udpMgm_3->nodeValue(canData);
        }
        if (PASS_2 == canData.value(1)) {
            QList<int> canTempData = canData;
            canTempData[0] = canData.value(0) + m_pass1Count;
            m_udpMgm_3->nodeValue(canTempData);
        }
    }

}
void MainPage::confNodeInfo()
{

    QSqlDatabase db = SQLite::openConnection();
    QList<QStringList> nodeStringList_1 = SQLite::getNodeInfoList(db,QString::number(PASS_1));
    QList<QStringList> nodeStringList_2 = SQLite::getNodeInfoList(db,QString::number(PASS_2));
    SQLite::closeConnection(db);

    ui->tBtnPass1->setVisible(false);
    ui->tBtnPass2->setVisible(false);

    QSettings settings("./Conf.ini", QSettings::IniFormat);
    //配置CANNET
    QString pThost  = settings.value("CANNET/THOST_1").toString();
    quint16 pTport_1= settings.value("CANNET/TPORT_1").toInt();
    quint16 pTport_2= settings.value("CANNET/TPORT_2").toInt();
    QString pLhost  = settings.value("CANNET/LHOST_1").toString();
    quint16 pLport_1= settings.value("CANNET/LPORT_1").toInt();
    quint16 pLport_2= settings.value("CANNET/LPORT_2").toInt();
    int     pTimes_1 =settings.value("CANNET/TIMES_1").toInt();
    int     pTimes_2 =settings.value("CANNET/TIMES_2").toInt();
    m_canType = settings.value("CANNET/CANTYPE").toInt();

    m_udpFlag_1 = settings.value("NETWORK_1/UDPFLAG").toInt();
    if (1 == m_udpFlag_1) {
        QString pHostIP = settings.value("NETWORK_1/HOST").toString();
        quint16 pPort = settings.value("NETWORK_1/PORT").toUInt();
        m_udpMgm_1 = new UDPMgm;
        m_udpMgm_1->initConf(QHostAddress(pHostIP),pPort);
        connect(m_udpMgm_1,SIGNAL(sigMute()),this,SLOT(slotBtnMute()));
        connect(m_udpMgm_1,SIGNAL(sigReset()),this,SLOT(slotUdpReset()));
    }

    m_udpFlag_2 = settings.value("NETWORK_2/UDPFLAG").toInt();
    if (1 == m_udpFlag_2) {
        QString pHostIP = settings.value("NETWORK_2/HOST").toString();
        quint16 pPort = settings.value("NETWORK_2/PORT").toUInt();
        m_udpMgm_2 = new UDPMgm;
        m_udpMgm_2->initConf(QHostAddress(pHostIP),pPort);
        connect(m_udpMgm_2,SIGNAL(sigMute()),this,SLOT(slotBtnMute()));
        connect(m_udpMgm_2,SIGNAL(sigReset()),this,SLOT(slotUdpReset()));
    }

    m_udpFlag_3 = settings.value("NETWORK_3/UDPFLAG").toInt();
    if (1 == m_udpFlag_3) {
        QString pHostIP = settings.value("NETWORK_3/HOST").toString();
        quint16 pPort = settings.value("NETWORK_3/PORT").toUInt();
        m_udpMgm_3 = new UDPMgm;
        m_udpMgm_3->initConf(QHostAddress(pHostIP),pPort);
        connect(m_udpMgm_3,SIGNAL(sigMute()),this,SLOT(slotBtnMute()));
        connect(m_udpMgm_3,SIGNAL(sigReset()),this,SLOT(slotUdpReset()));
    }
    if (CANUSB == m_canType) {
        CanDevice::openCanBusDev(USBCAN);
    }

    m_pass1Count = nodeStringList_1.count();
    if (m_pass1Count > 0) {
        ui->tBtnPass1->setVisible(true);
        m_nodeView_1 = new NodeView;
        m_nodeView_1->nodeConf(PASS_1,nodeStringList_1);
        ui->stackedWgtPass->addWidget(m_nodeView_1);
        connect(m_nodeView_1,&NodeView::sigInfoCount,this,&MainPage::slotNodeInfoCount);
        connect(m_nodeView_1,&NodeView::sigThreadRestart,this,&MainPage::slotThreadRestart);
        if (CANUSB == m_canType) {
            m_canBusThread_1 = new CanBusThread(USBCAN,PASS_1);
            connect(m_canBusThread_1,SIGNAL(sigThreadPaused(int)),this,SLOT(slotThreadPaused(int)));
            connect(m_canBusThread_1,SIGNAL(sigOffLineTimes(int,bool)),this,SLOT(slotOffLine(int,bool)));
            connect(m_canBusThread_1,SIGNAL(sigSendCanData(QList<int>)),m_RS485,SLOT(slotNodeState(QList<int>)));
            connect(m_canBusThread_1,SIGNAL(sigSendCanData(QList<int>)),this,SLOT(slotRecvCanData(QList<int>)));
            connect(m_canBusThread_1,SIGNAL(sigSendCanData(QList<int>)),this,SLOT(slotSendUdpData(QList<int>)));
            m_canBusThread_1->start();
        } else if (CANNET == m_canType) {
            m_canNetUDP_1 = new CanNetUDP(QHostAddress(pThost),QHostAddress(pLhost));
            m_canNetUDP_1->confCanNetUDP(pTport_1,pLport_1,PASS_1,pTimes_1);
            QThread *thread = new QThread;
            m_canNetUDP_1->moveToThread(thread);
            thread->start();
            connect(m_canNetUDP_1,SIGNAL(sigThreadPaused(int)),this,SLOT(slotThreadPaused(int)));
            connect(m_canNetUDP_1,SIGNAL(sigOffLineTimes(int,bool)),this,SLOT(slotOffLine(int,bool)));
            connect(m_canNetUDP_1,SIGNAL(sigSendCanData(QList<int>)),m_RS485,SLOT(slotNodeState(QList<int>)));
            connect(m_canNetUDP_1,SIGNAL(sigSendCanData(QList<int>)),this,SLOT(slotRecvCanData(QList<int>)));
            connect(m_canNetUDP_1,SIGNAL(sigSendCanData(QList<int>)),this,SLOT(slotSendUdpData(QList<int>)));
        }
    }

    m_pass2Count = nodeStringList_2.count();
    if (m_pass2Count > 0) {
        ui->tBtnPass2->setVisible(true);
        m_nodeView_2 = new NodeView;
        m_nodeView_2->nodeConf(PASS_1,nodeStringList_2);;
        ui->stackedWgtPass->addWidget(m_nodeView_2);
        connect(m_nodeView_2,&NodeView::sigInfoCount,this,&MainPage::slotNodeInfoCount);
        connect(m_nodeView_2,&NodeView::sigThreadRestart,this,&MainPage::slotThreadRestart);

        if (CANUSB == m_canType) {
            m_canBusThread_2 = new CanBusThread(USBCAN,PASS_2);
            connect(m_canBusThread_2,SIGNAL(sigThreadPaused(int)),this,SLOT(slotThreadPaused(int)));
            connect(m_canBusThread_2,SIGNAL(sigOffLineTimes(int,bool)),this,SLOT(slotOffLine(int,bool)));
            connect(m_canBusThread_2,SIGNAL(sigSendCanData(QList<int>)),m_RS485,SLOT(slotNodeState(QList<int>)));
            connect(m_canBusThread_2,SIGNAL(sigSendCanData(QList<int>)),this,SLOT(slotRecvCanData(QList<int>)));
            connect(m_canBusThread_2,SIGNAL(sigSendCanData(QList<int>)),this,SLOT(slotSendUdpData(QList<int>)));
            m_canBusThread_2->start();
        } else if (CANNET == m_canType) {

            m_canNetUDP_2 = new CanNetUDP(QHostAddress(pThost),QHostAddress(pLhost));
            m_canNetUDP_2->confCanNetUDP(pTport_2,pLport_2,PASS_2,pTimes_2);
            QThread *thread = new QThread;
            m_canNetUDP_2->moveToThread(thread);
            thread->start();
            connect(m_canNetUDP_2,SIGNAL(sigThreadPaused(int)),this,SLOT(slotThreadPaused(int)));
            connect(m_canNetUDP_2,SIGNAL(sigOffLineTimes(int,bool)),this,SLOT(slotOffLine(int,bool)));
            connect(m_canNetUDP_2,SIGNAL(sigSendCanData(QList<int>)),m_RS485,SLOT(slotNodeState(QList<int>)));
            connect(m_canNetUDP_2,SIGNAL(sigSendCanData(QList<int>)),this,SLOT(slotRecvCanData(QList<int>)));
            connect(m_canNetUDP_2,SIGNAL(sigSendCanData(QList<int>)),this,SLOT(slotSendUdpData(QList<int>)));
        }
    }
}

void MainPage::controlCanLed()
{
    m_controlLight->controlCanLed(true);
    m_controlLight->controlCanLed(false);
}

void MainPage::confUserType(int userType)
{
    ui->tBtnUserLogin->setVisible(false);
    ui->tBtnUserLogout->setVisible(true);
    if (ADMIN == userType) {
        m_userType = userType;
        ui->tBtnReset->setEnabled(true);
        ui->tBtnAlarmSet->setEnabled(false);
        ui->tBtnSelfCheck->setEnabled(true);
        ui->tBtnRecordQuery->setEnabled(true);
    } else if (SUPER == userType) {
        m_userType = userType;
        ui->tBtnReset->setEnabled(true);
        ui->tBtnAlarmSet->setEnabled(true);
        ui->tBtnSelfCheck->setEnabled(true);
        ui->tBtnRecordQuery->setEnabled(true);
    }
}

void MainPage::slotTimeOut()
{
    if (true == m_selfCheckFlag) {
        m_selfCheck->selfCheckStart();
    } else {
        controlCanLed();
        controlSoundLed();
    }
}

void MainPage::slotOffLine(int CANIndex, bool flag)
{
    if (PASS_1 == CANIndex+1) {
        can_1_flag = flag;
    }

    if (PASS_2 == CANIndex+1) {
        can_2_flag = flag;
    }

    if (true == can_1_flag && true == can_2_flag) {
        can_1_flag = false;
        can_2_flag = false;
        CanDevice::closeCanBusDev(USBCAN);
        QThread::msleep(3000);
        CanDevice::openCanBusDev(USBCAN);
    }
}

void MainPage::slotRecvCanData(QList<int> canData)
{
    if (PASS_1 == canData.value(1)) {
        m_nodeView_1->recvCanData(canData);
    }
    if (PASS_2 == canData.value(1)) {
        m_nodeView_2->recvCanData(canData);
    }
}

void MainPage::initConnect()
{
    m_timer = new QTimer;
    connect(m_timer,SIGNAL(timeout()),this,SLOT(slotTimeOut()));
    m_timer->start(2000);

    connect(ui->tBtnPass1,&QToolButton::clicked,this,&MainPage::slotBtnPass1);
    connect(ui->tBtnPass2,&QToolButton::clicked,this,&MainPage::slotBtnPass2);

    connect(ui->tBtnMute, &QToolButton::clicked,this,&MainPage::slotBtnMute);
    connect(ui->tBtnReset,&QToolButton::clicked,this,&MainPage::slotBtnReset);      //软件复位
    connect(m_controlLight,&ControlLight::sigReset,this,&MainPage::slotBtnReset);   //硬件复位
    connect(ui->tBtnAlarmSet,&QToolButton::clicked,this,&MainPage::sigBtnNodeInfo);
    connect(ui->tBtnUserLogin,  &QToolButton::clicked,this,&MainPage::sigBtnLogin);
    connect(ui->tBtnRecordQuery,&QToolButton::clicked,this,&MainPage::sigBtnRecord);
    connect(ui->tBtnUserLogout, &QToolButton::clicked,this,&MainPage::slotBtnLogout);
    connect(ui->tBtnSelfCheck,&QToolButton::clicked,this,&MainPage::slotBtnSelfCheck);
    connect(m_controlLight,&ControlLight::sigPowerState,m_RS485,&RS485::slotPowerState);
    connect(m_controlLight,&ControlLight::sigPowerState,this,&MainPage::slotPowerState);
    connect(m_selfCheck,&SelfCheck::sigSelfCheckFinish,this,&MainPage::slotBtnSelfCheck);
    connect(m_selfCheck,&SelfCheck::sigControlSound,m_controlLight,&ControlLight::slotControlSound);
    connect(m_selfCheck,&SelfCheck::sigControlLight,m_controlLight,&ControlLight::slotSendDataSerial);
    connect(this,&MainPage::sigConfSerialData,m_controlLight,&ControlLight::slotConfSerialData);


}

void MainPage::initVariable()
{
    QSettings settings("./Conf.ini", QSettings::IniFormat);
    m_hostArea = settings.value("HOSTAREA/HostArea").toString();
    m_RS485 = new RS485;
    m_selfCheck = new SelfCheck;
    m_controlLight = new ControlLight;

    insertList();
    m_times = 0;
    m_userType = USER;
    m_alarmCount = 0;
    m_errorCount = 0;
    m_pass1Count = 0;
    m_pass2Count =  0;
    m_muteFlag = false;
    m_mainPower = true;
    m_backPower = true;
    can_1_flag = false;
    can_2_flag = false;
    m_backPowerFlag = false;
    m_mainPowerFlag = false;
    m_selfCheckFlag = false;
    ui->tBtnAlarmSet->setVisible(false);
    ui->tBtnReset->setEnabled(false);
    ui->tBtnAlarmSet->setEnabled(false);
    ui->tBtnSelfCheck->setEnabled(false);
    ui->tBtnRecordQuery->setEnabled(false);
}

void MainPage::slotBtnMute()
{

    m_muteFlag = true;
    m_controlLight->setNormalSound();

    //    QSqlDatabase db = SQLite::openConnection();
    //    for(int id = 1; id < 2034;id += 8){
    //        SQLite::insertNode(db,QString::number(1),QString::number(id),QString("2"),QString("1"),QString(" "));
    //    }
    //    for(int id = 1; id < 2034;id += 8){
    //        SQLite::insertNode(db,QString::number(2),QString::number(id),QString("2"),QString("1"),QString(" "));
    //    }
    //    SQLite::closeConnection(db);
}

void MainPage::slotBtnReset()
{
    //所有节点状态恢复正常,节点记录全部清空
    //线程停止接收数据
    //声音恢复正常
    //指示灯恢复正常
    if (USER == m_userType) {
        MsgBox::showInformation(this,tr("操作提示"),tr("你没有权限操作此项功能!"),tr("关闭"));
        return;
    }

    int ret = MsgBox::showQuestion(this,tr("操作提示"),tr("你确定要复位所有节点？"),tr("确定"),tr("取消"));
    if (!ret) {
        if (ui->tBtnPass1->isVisible() == true) {
            if (CANUSB == m_canType) {
                m_canBusThread_1->threadPause();
            } else if(CANNET == m_canType) {
                m_canNetUDP_1->timerPause();
            }
        }
        if (ui->tBtnPass2->isVisible() == true) {
            if (CANUSB == m_canType) {
                m_canBusThread_2->threadPause();
            } else if(CANNET == m_canType) {
                m_canNetUDP_2->timerPause();
            }
        }
        m_controlLight->controlReset();//所有指示灯都灭
    }
}

void MainPage::slotUdpReset()
{
    if (ui->tBtnPass1->isVisible() == true) {
        if (CANUSB == m_canType) {
            m_canBusThread_1->threadPause();
        } else if(CANNET == m_canType) {
            m_canNetUDP_1->timerPause();
        }
    }
    if (ui->tBtnPass2->isVisible() == true) {
        if (CANUSB == m_canType) {
            m_canBusThread_2->threadPause();
        } else if(CANNET == m_canType) {
            m_canNetUDP_2->timerPause();
        }
    }
    m_controlLight->controlReset();//所有指示灯都灭
}

void MainPage::slotNodeInfoCount(int pass, int errorCount, int alarmCount)
{
    setErrorCount(pass,errorCount);
    setAlarmCount(pass,alarmCount);
}

void MainPage::slotBtnSelfCheck()
{
    if (false == m_selfCheckFlag) {
        m_selfCheckFlag = true;
        m_controlLight->controlDataTimer(false);
        m_selfCheck->selfCheckStart();
    } else if (true == m_selfCheckFlag) {
        m_selfCheckFlag = false;
        m_controlLight->controlDataTimer(true);
    }
}


void MainPage::setErrorCount(int pass, int errorCount)
{
    if (errorCount > m_errorCountList.value(pass)) {
        m_muteFlag = false;
    }
    m_errorCountList.replace(pass,errorCount);
    int errorTotal = 0;
    for (int  i = 0; i< m_errorCountList.count();i++) {
        errorTotal += m_errorCountList.value(i);
    }
    m_errorCount = errorTotal;
}

void MainPage::setAlarmCount(int pass, int alarmCount)
{
    //报警的数目只会增加，只需要更新数目就行
    if (alarmCount > m_alarmCountList.value(pass)) {
        m_muteFlag = false;
    }
    m_alarmCountList.replace(pass,alarmCount);
    int alarmTotal = 0;
    for (int  i = 0; i < m_alarmCountList.count();i++) {
        alarmTotal += m_alarmCountList.value(i);
    }
    m_alarmCount = alarmTotal;

}

void MainPage::slotThreadPaused(int CANIndex)
{
    m_muteFlag = false;
    m_mainPower = true;
    m_backPower = true;
    m_mainPowerFlag = false;
    m_backPowerFlag = false;
    m_alarmCount = 0;
    m_errorCount = 0;
    replaceList(CANIndex - 1);
    if (PASS_1 == CANIndex) {
        m_nodeView_1->nodeReset();
    }
    if (PASS_2 == CANIndex) {
        m_nodeView_2->nodeReset();
    }

}

void MainPage::slotThreadRestart(int CANIndex)
{
    if (PASS_1 == CANIndex && ui->tBtnPass1->isVisible()) {
        if (CANUSB == m_canType) {
            m_canBusThread_1->threadRestart();
        } else if(CANNET == m_canType) {
            m_canNetUDP_1->timerRestart();
        }
    }
    if (PASS_2 == CANIndex && ui->tBtnPass2->isVisible()) {
        if (CANUSB == m_canType) {
            m_canBusThread_2->threadRestart();
        } else if(CANNET == m_canType) {
            m_canNetUDP_2->timerRestart();
        }
    }
}

void MainPage::slotPowerState(int mainPower, int backPower)
{
    if (1 == mainPower) {
        m_mainPower = true;
    } else {
        m_mainPower = false;
    }

    if (1 == backPower) {
        m_backPower = true;
    } else {
        m_backPower = false;
    }
}

void MainPage::slotSendAlarmData(int pass, quint32 ID, quint16 type, quint16 alarm, quint16 base)
{
    if (pass == 1 && ui->tBtnPass1->isEnabled()) {
        m_canBusThread_1->controlAlarmData(ID,type,alarm,base);
    } else if (pass == 2 && ui->tBtnPass2->isEnabled()) {
        m_canBusThread_2->controlAlarmData(ID,type,alarm);
    }
}

void MainPage::slotBtnLogout()
{
    m_userType = USER;
    ui->tBtnUserLogin->setVisible(true);
    ui->tBtnUserLogout->setVisible(false);

    ui->tBtnReset->setEnabled(false);
    ui->tBtnSelfCheck->setEnabled(false);
    ui->tBtnRecordQuery->setEnabled(false);
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
