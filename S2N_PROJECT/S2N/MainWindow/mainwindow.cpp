#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Record/record.h"
#include "MainPage/mainpage.h"
#include "CanBusThread/candevice.h"
#include "CanBusThread/canbusthread.h"

#include <Login/login.h>
#include <NodeInfo/nodeinfo.h>
#include <QDateTime>
#include <QSettings>
#include <QTimer>

#define MAINPAGE    0
#define RECORD      1
#define LOGIN       2
#define NODEINFO    3
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //showFullScreen();
    setWindowFlags(Qt::FramelessWindowHint);
    m_login = new Login;
    m_record = new Record;
    m_mainPage = new MainPage;
    m_nodeInfo = new NodeInfo;
    ui->stackedWidget->addWidget(m_mainPage);
    ui->stackedWidget->addWidget(m_record);
    ui->stackedWidget->addWidget(m_login);
    ui->stackedWidget->addWidget(m_nodeInfo);
    initWidget();
    initConncet();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initConncet()
{
    connect(m_mainPage,SIGNAL(sigBtnLogin()),this,SLOT(slotBtnLogin()));
    connect(m_mainPage,SIGNAL(sigBtnRecord()),this,SLOT(slotBtnRecord()));
    connect(m_mainPage,SIGNAL(sigBtnNodeInfo()),this,SLOT(slotBtnNodeInfo()));

    connect(m_login,SIGNAL(sigLoginOk(int)),this,SLOT(slotLoginOk(int)));
    connect(m_login,SIGNAL(sigNodeView()),this,SLOT(slotBtnNodeView()));
    connect(m_nodeInfo,SIGNAL(sigNodeView()),this,SLOT(slotBtnNodeView()));
    connect(m_record,SIGNAL(sigNodeView()),this,SLOT(slotBtnNodeView()));

    connect(m_nodeInfo,SIGNAL(sigSendAlarmData(int,quint32,quint16,quint16,quint16)),
            m_mainPage,SLOT(slotSendAlarmData(int,quint32,quint16,quint16,quint16)));

    m_systemTime = new QTimer;
    connect(m_systemTime,&QTimer::timeout,this,&MainWindow::slotSystemTime);
    m_systemTime->start(1000);

}

void MainWindow::initWidget()
{
    QSettings settings("./Conf.ini", QSettings::IniFormat);
    int company = settings.value("COMPANY/Company").toInt();
    if (1 == company) {
        ui->lbLogo->setStyleSheet("border-image: url(:/Image/Sensor.png);");
        ui->lbCompany->setText(tr("西安盛赛尔电子有限公司"));
    } else if (2 == company) {
        ui->lbLogo->setStyleSheet("border-image: url(:/Image/Bevone.png);");
        ui->lbCompany->setText(tr("北京北元安达电子有限公司"));
    }
    ui->lbTime->setText(QDateTime::currentDateTime().toString("yyyy年MM月dd日 hh:mm:ss"));
}

void MainWindow::slotBtnLogin()
{
    m_login->confShow();
    ui->stackedWidget->setCurrentIndex(LOGIN);
}

void MainWindow::slotBtnRecord()
{
    m_record->confShow(m_userType);
    ui->stackedWidget->setCurrentIndex(RECORD);
}

void MainWindow::slotBtnNodeView()
{
    ui->stackedWidget->setCurrentIndex(MAINPAGE);
}

void MainWindow::slotBtnNodeInfo()
{
    ui->stackedWidget->setCurrentIndex(NODEINFO);
}

void MainWindow::slotLoginOk(int userType)
{
    m_userType = userType;
    m_mainPage->confUserType(userType);
    ui->stackedWidget->setCurrentIndex(MAINPAGE);
}

void MainWindow::slotSystemTime()
{
    ui->lbTime->setText(QDateTime::currentDateTime().toString("yyyy年MM月dd日 hh:mm:ss"));
}
