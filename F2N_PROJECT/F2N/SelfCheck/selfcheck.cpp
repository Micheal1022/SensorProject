#include "selfcheck.h"
#include "ui_selfcheck.h"
#include "MyPrint/myprint.h"
#include <QTimer>
#define TIME 1000


SelfCheck::SelfCheck(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelfCheck)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    //showFullScreen();
    this->hide();
    m_colorType = 0;
    m_redStr    = QStringLiteral("background-color:rgb(255, 0, 0);");
    m_blueStr   = QStringLiteral("background-color:rgb(0, 0, 255);");
    m_greenStr  = QStringLiteral("background-color:rgb(0, 255, 0);");
    m_whiteStr  = QStringLiteral("background-color:rgb(255, 255, 255);");
    m_yellowStr = QStringLiteral("background-color:rgb(255, 255, 0);");

    m_colorTimer = new QTimer;
    connect(m_colorTimer,&QTimer::timeout,this,&SelfCheck::slotColorTimeOut);
}

SelfCheck::~SelfCheck()
{
    delete ui;
}

void SelfCheck::slotColorTimeOut()
{
    switch (m_colorType) {
    case 0:
        emit sigControlSound(1);
        setStyleSheet(m_redStr);
        emit sigControlLight(0xFF,1);
        break;
    case 1:
        setStyleSheet(m_greenStr);
        emit sigControlLight(0x01,0);
        break;
    case 2:
        setStyleSheet(m_blueStr);
        emit sigControlLight(0x02,0);
        break;
    case 3:
        setStyleSheet(m_yellowStr);
        emit sigControlLight(0x03,0);
        break;
    case 4:
        setStyleSheet(m_whiteStr);
        emit sigControlLight(0x04,0);
        break;
    case 5:
        setStyleSheet(m_redStr);
        emit sigControlLight(0x06,0);
        break;
    case 6:
        setStyleSheet(m_greenStr);
        emit sigControlLight(0x05,0);
        break;
    case 7:
        setStyleSheet(m_blueStr);
        emit sigControlLight(0x07,0);
        break;
    case 8:
        setStyleSheet(m_yellowStr);
        emit sigControlLight(0x08,0);
        break;
    case 9:
        setStyleSheet(m_whiteStr);
        emit sigControlLight(0x09,0);
        break;
    }

    m_colorType++;
    if(m_colorType == 10)
    {
        hide();
        m_colorType = 0;
        m_colorTimer->stop();
        MyPrint::selfCheck();
        emit sigSelfCheckFinish();
    }
}

void SelfCheck::selfCheckStart()
{
    m_colorTimer->start(TIME);
    this->setGeometry(0,0,1024,768);
    show();
}
