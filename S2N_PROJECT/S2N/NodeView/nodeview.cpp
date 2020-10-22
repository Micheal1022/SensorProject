#include "nodeview.h"
#include "ui_nodeview.h"
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QStringList>
#include <QButtonGroup>
#include "SQLite/sqlite.h"
#include "infoview.h"
#include "CanBusThread/canbusthread.h"

//module status
#define MOD_NORMAL  0x00//正常
#define MOD_ALARM   0x01//报警
#define MOD_ERROR   0x02//故障
#define MOD_OFFLINE 0x03//掉线

#define PASS    0
#define CANID   1
#define TYPE    2
#define AREA    3

#define COLUMNSIZE  15
#define PAGESIZE    400

NodeView::NodeView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NodeView)
{
    ui->setupUi(this);
    lcdNumberClear();
    m_tBtnUnitInfoList.clear();
    initLayout();
    initConnect();
}

NodeView::~NodeView()
{
    delete ui;
}

void NodeView::nodeConf(int pass, QList<QStringList> stringList)
{
    m_curBtn = 0;
    m_pass = pass;
    m_currentPage = 0;
    m_nodeCount = stringList.count();
    m_pageCount = getPageCount(stringList.count());
    m_nodeErrorList.clear();
    m_nodeAlarmList.clear();
    if (m_pageCount > 0) {
        ui->tBtnHeadPage->setEnabled(false);
        ui->tBtnLastPage->setEnabled(false);
        ui->tBtnTailPage->setEnabled(true);
        ui->tBtnNestPage->setEnabled(true);
    } else {
        ui->tBtnHeadPage->setEnabled(false);
        ui->tBtnTailPage->setEnabled(false);
        ui->tBtnNestPage->setEnabled(false);
        ui->tBtnLastPage->setEnabled(false);
    }
    ui->lbNodeCount->setText(QString::number(stringList.count()));
    initNodeBtn(stringList);
}

void NodeView::recvCanData(QList<int> canData)
{
    int index = canData.value(0);
    QString nodeArea = m_tBtnUnitInfoList[index].m_nodeArea;
    int nodeID    = canData.value(2);
    int nodeType  = canData.value(3);
    int nodeState = canData.value(4);
    m_tBtnUnitInfoList[index].m_nodeID   = nodeID;
    m_tBtnUnitInfoList[index].m_nodeType = nodeType;
    m_tBtnUnitInfoList[index].m_nodeState= nodeState;
    ui->lbCurCanId->setText(QString::number(nodeID));
    //报警时间
    QDateTime currentTime = QDateTime::currentDateTime();
    QString alarmTime = currentTime.toString(tr("yyyy年MM月dd日 hh:mm:ss"));

    QString btnText = m_btnGroup->button(index)->text();
    btnText = btnText.left(btnText.indexOf("\n")+1)+btnSetText(nodeType);
    m_btnGroup->button(index)->setText(btnText);

    switch (nodeType) {
    case MOD_LEAK:
        m_tBtnUnitInfoList[index].m_realLeak = canData[5];
        m_tBtnUnitInfoList[index].m_alarmLeak= canData[6];
        m_tBtnUnitInfoList[index].m_baseLeak = canData[7];
        break;
    case MOD_TEMP:
        m_tBtnUnitInfoList[index].m_realTemp = canData[5];
        m_tBtnUnitInfoList[index].m_alarmTemp= canData[6];
        break;
    }

    switch (nodeState) {
    case MOD_NORMAL://节点正常
        if (false == m_tBtnUnitInfoList[index].m_normalFlag ) {
            m_tBtnUnitInfoList[index].m_normalFlag  = true;
            m_tBtnUnitInfoList[index].m_errorFlag   = false;
            delStringList(m_nodeErrorList,m_pass,nodeID,MOD_ERROR);
            m_tBtnUnitInfoList[index].m_offLineFlag = false;
            delStringList(m_nodeErrorList,m_pass,nodeID,MOD_OFFLINE);

            if (true != m_tBtnUnitInfoList[index].m_alarmFlag) {
                m_btnGroup->button(index)->setStyleSheet("background-color: rgb(14, 221, 111);");
            }
        }
        break;
    case MOD_ERROR://节点故障
        if (false == m_tBtnUnitInfoList[index].m_errorFlag) {
            m_tBtnUnitInfoList[index].m_errorFlag   = true;
            m_tBtnUnitInfoList[index].m_normalFlag  = false;
            m_tBtnUnitInfoList[index].m_offLineFlag = false;

            delStringList(m_nodeErrorList,m_pass,nodeID,MOD_OFFLINE);
            if (true != m_tBtnUnitInfoList[index].m_alarmFlag) {
                m_btnGroup->button(index)->setStyleSheet("background-color: rgb(255, 233, 32);");
            }
            QStringList nodeErrorList = addStringList(m_pass,nodeID,nodeType,nodeState,0,alarmTime,nodeArea);
            m_nodeErrorList.append(nodeErrorList);
            QSqlDatabase db = SQLite::openConnection();
            SQLite::insertRecord(db,m_pass,nodeID,nodeType,nodeState,0,currentTime.toTime_t(),nodeArea);
            SQLite::closeConnection(db);
        }
        break;
    case MOD_ALARM://节点报警
        if (false == m_tBtnUnitInfoList[index].m_alarmFlag) {
            m_tBtnUnitInfoList[index].m_alarmFlag   = true;
            m_tBtnUnitInfoList[index].m_normalFlag  = false;
            m_tBtnUnitInfoList[index].m_offLineFlag = false;

            delStringList(m_nodeAlarmList,m_pass,nodeID,MOD_OFFLINE);
            m_btnGroup->button(index)->setStyleSheet("background-color: rgb(255, 77, 70);");
            int alarmValue  = canData[5];
            switch (nodeType) {
            case MOD_LEAK:
                m_tBtnUnitInfoList[index].m_lockLeak = alarmValue;
                break;
            case MOD_TEMP:
                m_tBtnUnitInfoList[index].m_lockTemp = alarmValue;
                break;
            }

            QStringList nodeAlarmList = addStringList(m_pass,nodeID,nodeType,nodeState,alarmValue,alarmTime,nodeArea);
            m_nodeAlarmList.append(nodeAlarmList);

            QSqlDatabase db = SQLite::openConnection();
            SQLite::insertRecord(db,m_pass,nodeID,nodeType,nodeState,alarmValue,currentTime.toTime_t(),nodeArea);
            SQLite::closeConnection(db);
        }
        break;
    case MOD_OFFLINE://节点掉线
        if (false == m_tBtnUnitInfoList[index].m_offLineFlag) {
            m_tBtnUnitInfoList[index].m_offLineFlag = true;
            m_tBtnUnitInfoList[index].m_normalFlag  = false;

            delStringList(m_nodeErrorList,m_pass,nodeID,MOD_ERROR);
            if (true != m_tBtnUnitInfoList[index].m_alarmFlag) {
                m_btnGroup->button(index)->setStyleSheet("background-color: rgb(171, 171, 171);");
            }

            switch (nodeType) {
            case MOD_LEAK:
                m_tBtnUnitInfoList[index].m_realLeak = 0;
                break;
            case MOD_TEMP:
                m_tBtnUnitInfoList[index].m_realTemp = 0;
                break;
            }

            QStringList nodeOffLineList = addStringList(m_pass,nodeID,nodeType,nodeState,0,alarmTime,nodeArea);
            m_nodeErrorList.append(nodeOffLineList);

            QSqlDatabase db = SQLite::openConnection();
            SQLite::insertRecord(db,m_pass,nodeID,nodeType,nodeState,0,currentTime.toTime_t(),nodeArea);
            SQLite::closeConnection(db);
        }
        break;
    }
    emit sigInfoCount(m_pass-1,m_nodeErrorList.count(),m_nodeAlarmList.count());
}


void NodeView::initLayout()
{
    QVBoxLayout *verticalLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    verticalLayout->setSpacing(5);
    verticalLayout->setContentsMargins(10, 0, 10, 0);

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(5);

    m_gridLayout = new QGridLayout();
    m_gridLayout->setHorizontalSpacing(15);
    m_gridLayout->setVerticalSpacing(5);
    m_gridLayout->setContentsMargins(10, 5, 0, 10);

    horizontalLayout->addLayout(m_gridLayout);
    QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);

    verticalLayout->addLayout(horizontalLayout);
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(verticalSpacer);

    ui->scrollArea->verticalScrollBar()->setSingleStep(50*8);
}

void NodeView::initConnect()
{
    m_timer = new QTimer;
    connect(m_timer,SIGNAL(timeout()),this,SLOT(slotTimeOut()));
    m_timer->start(1000);
    connect(this,SIGNAL(sigSetValue(int)),ui->scrollArea->verticalScrollBar(),SLOT(setValue(int)));
    connect(ui->tBtnHeadPage, SIGNAL(clicked(bool)),this,SLOT(slotBtnHeadPage()));
    connect(ui->tBtnTailPage, SIGNAL(clicked(bool)),this,SLOT(slotBtnTailPage()));
    connect(ui->tBtnLastPage, SIGNAL(clicked(bool)),this,SLOT(slotBtnLastPage()));
    connect(ui->tBtnNestPage, SIGNAL(clicked(bool)),this,SLOT(slotBtnNestPage()));
    connect(ui->tBtnAlarmInfo,SIGNAL(clicked(bool)),this,SLOT(slotBtnAlarmInfo()));
    connect(ui->tBtnErrorInfo,SIGNAL(clicked(bool)),this,SLOT(slotBtnErrorInfo()));
}

void NodeView::lcdNumberClear()
{
    ui->lbArea->clear();
    ui->lbNodeType->clear();
    ui->lbNodeCanId->clear();
    ui->lbNodeState->clear();
    ui->lcdNbRealLeak ->display(0);
    ui->lcdNbLockLeak->display(0);
    ui->lcdNbBaseLeak->display(0);
    ui->lcdNbRealTemp->display(0);
    ui->lcdNbLockLeak->display(0);
}

void NodeView::showCurrentNodeValue(int curBtn)
{
    lcdNumberClear();
    ui->lbArea->setText(m_tBtnUnitInfoList.at(curBtn).m_nodeArea);
    ui->lbNodeCanId->setText(QString::number(m_pass)+"-"+QString::number(m_tBtnUnitInfoList.at(curBtn).m_nodeID));

    int state = m_tBtnUnitInfoList.at(curBtn).m_nodeState;
    switch (state) {
    case MOD_ALARM:
        ui->lbNodeState->setText("节点报警");
        break;
    case MOD_ERROR:
        ui->lbNodeState->setText("节点故障");
        break;
    case MOD_OFFLINE:
        ui->lbNodeState->setText("通讯故障");
        break;
    case MOD_NORMAL:
        ui->lbNodeState->setText("节点正常");
        break;
    }

    int nodeType = m_tBtnUnitInfoList.at(curBtn).m_nodeType;
    switch (nodeType) {
    case MOD_LEAK:
        ui->lbNodeType->setText("漏电");
        ui->stackedWidget->setVisible(true);
        ui->stackedWidget->setCurrentWidget(ui->leakPage);
        ui->lcdNbRealLeak->display(m_tBtnUnitInfoList[curBtn].m_realLeak);
        ui->lcdNbBaseLeak->display(m_tBtnUnitInfoList[curBtn].m_baseLeak);
        ui->lcdNbLockLeak->display(m_tBtnUnitInfoList[curBtn].m_lockLeak);
        ui->lcdNbAlarmLeak->display(m_tBtnUnitInfoList[curBtn].m_alarmLeak);
        break;
    case MOD_TEMP:
        ui->lbNodeType->setText("温度");
        ui->stackedWidget->setVisible(true);
        ui->stackedWidget->setCurrentWidget(ui->tempPage);
        ui->lcdNbRealTemp->display(m_tBtnUnitInfoList[curBtn].m_realTemp);
        ui->lcdNbLockTemp->display(m_tBtnUnitInfoList[curBtn].m_lockTemp);
        ui->lcdNbAlarmTemp->display(m_tBtnUnitInfoList[curBtn].m_alarmTemp);
        break;
    case MOD_EARC:
        ui->lbNodeType->setText("电弧");
        ui->stackedWidget->setVisible(false);
        break;
    }
}

QString NodeView::btnSetText(int nodeType)
{
    QString btnText;
    switch (nodeType) {
    case MOD_UREG:
        btnText = tr("未注册");
        break;
    case MOD_LEAK:
        btnText = tr("漏电");
        break;
    case MOD_TEMP:
        btnText = tr("温度");
        break;
    case MOD_EARC:
        btnText = tr("电弧");
        break;
    }
    return btnText;
}

void NodeView::initNodeBtn(QList<QStringList> stringList)
{
    m_btnGroup = new QButtonGroup;
    for (int index = 0; index < stringList.count(); index++) {
        BtnUnitInfo btnUnitInfo;
        btnUnitInfo.initData();
        QString nodeType = stringList.value(index).at(TYPE);
        QString nodeID = stringList.value(index).at(CANID);
        btnUnitInfo.m_nodeID   = nodeID.toInt();
        btnUnitInfo.m_nodeType = nodeType.toInt();
        btnUnitInfo.m_nodeArea = stringList.value(index).at(AREA);
        m_tBtnUnitInfoList.append(btnUnitInfo);

        QToolButton *tBtn = new QToolButton(ui->scrollAreaWidgetContents);
        tBtn->setEnabled(true);
        tBtn->setFocusPolicy(Qt::ClickFocus);
        tBtn->setMinimumSize(QSize(50, 45 ));
        tBtn->setMaximumSize(QSize(50, 45));
        tBtn->setStyleSheet("background-color: rgb(14, 221, 111);");
        tBtn->setText(stringList.at(index).at(CANID)+"\n"+btnSetText(nodeType.toInt()));
        m_btnGroup->addButton(tBtn,index);
        m_gridLayout->addWidget(tBtn, index / COLUMNSIZE, index % COLUMNSIZE, 1, 1);
    }
    connect(m_btnGroup,SIGNAL(buttonClicked(int)),this,SLOT(slotCurrentBtn(int)));
}

int NodeView::getPageCount(int count)
{
    int rowCount = count / 15;
    if (count % 15 > 0) {
        rowCount += 1;
    }
    int pageCount = rowCount / 9;
    if(pageCount % 9)
        pageCount += 1;
    return pageCount;
}

QStringList NodeView::addStringList(int pass, int ID, int type, int state, int alarmValue,QString alarmTime,QString area)
{
    QString passStr = QString::number(pass);
    QString IDStr= QString::number(ID);
    QString alarmStr = QString::number(alarmValue);
    QString stateStr,typeStr;
    switch (state) {
    case MOD_ERROR:
        stateStr = QString(tr("节点故障"));
        break;
    case MOD_ALARM:
        stateStr = QString(tr("节点报警"));
        break;
    case MOD_OFFLINE:
        stateStr = QString(tr("通讯故障"));
        break;
    }

    switch (type) {
    case MOD_LEAK:
        typeStr = QString(tr("漏电"));
        break;
    case MOD_TEMP:
        typeStr = QString(tr("温度"));
        break;
    case MOD_EARC:
        typeStr = QString(tr("电弧"));
        break;
    }

    QStringList strList;
    strList<<passStr<<IDStr<<typeStr<<stateStr<<alarmStr<<alarmTime<<area;
    return strList;
}

void NodeView::delStringList(QList<QStringList> &stringlist, int pass, int ID, int state)
{
    QString passStr = QString::number(pass);
    QString IDStr   = QString::number(ID);
    QString stateStr;

    switch (state) {
    case MOD_ERROR:
        stateStr = QString(tr("节点故障"));
        break;
    case MOD_ALARM:
        stateStr = QString(tr("节点报警"));
        break;
    case MOD_OFFLINE:
        stateStr = QString(tr("通讯故障"));
        break;
    }

    for(int index = 0;index < stringlist.count();index++) {
        if (stringlist.at(index).at(0) == passStr && stringlist.at(index).at(1) == IDStr && stringlist.at(index).at(3) == stateStr) {
            stringlist.removeAt(index);
        }
    }
}

void NodeView::slotTimeOut()
{
    showCurrentNodeValue(m_curBtn);
}

void NodeView::slotCurrentBtn(int curBtn)
{
    m_curBtn = curBtn;
    showCurrentNodeValue(curBtn);
}

void NodeView::slotBtnHeadPage()
{
    m_currentPage = 0;
    emit sigSetValue(0);
    ui->tBtnLastPage->setEnabled(false);
    ui->tBtnNestPage->setEnabled(true);
    ui->tBtnHeadPage->setEnabled(false);
    ui->tBtnTailPage->setEnabled(true);
}

void NodeView::slotBtnTailPage()
{
    m_currentPage = m_pageCount;
    emit sigSetValue(m_currentPage * PAGESIZE);
    ui->tBtnNestPage->setEnabled(false);
    ui->tBtnLastPage->setEnabled(true);
    ui->tBtnTailPage->setEnabled(false);
    ui->tBtnHeadPage->setEnabled(true);
}

void NodeView::slotBtnLastPage()
{
    m_currentPage--;
    if (m_currentPage > 0) {
        emit sigSetValue(m_currentPage * PAGESIZE);
        ui->tBtnLastPage->setEnabled(true);
        ui->tBtnHeadPage->setEnabled(true);
        ui->tBtnNestPage->setEnabled(true);
        ui->tBtnTailPage->setEnabled(true);
    } else if (m_currentPage == 0) {
        emit sigSetValue(m_currentPage * PAGESIZE);
        ui->tBtnLastPage->setEnabled(false);
        ui->tBtnHeadPage->setEnabled(false);
        ui->tBtnNestPage->setEnabled(true);
        ui->tBtnTailPage->setEnabled(true);
    }
}

void NodeView::slotBtnNestPage()
{
    m_currentPage++;
    if (m_currentPage < m_pageCount) {
        emit sigSetValue(m_currentPage * PAGESIZE);
        ui->tBtnLastPage->setEnabled(true);
        ui->tBtnHeadPage->setEnabled(true);
        ui->tBtnNestPage->setEnabled(true);
        ui->tBtnTailPage->setEnabled(true);
    } else if (m_currentPage == m_pageCount) {
        emit sigSetValue(m_currentPage * PAGESIZE);
        ui->tBtnNestPage->setEnabled(false);
        ui->tBtnTailPage->setEnabled(false);
        ui->tBtnLastPage->setEnabled(true);
        ui->tBtnHeadPage->setEnabled(true);
    }
}

void NodeView::slotBtnAlarmInfo()
{
    InfoView infoView(this);
    infoView.initConf(m_nodeAlarmList, InfoView::AlarmData);
    infoView.exec();
}

void NodeView::slotBtnErrorInfo()
{
    InfoView infoView(this);
    infoView.initConf(m_nodeErrorList, InfoView::ErrorData);
    infoView.exec();
}

void NodeView::nodeReset()
{
    m_nodeErrorList.clear();
    m_nodeAlarmList.clear();
    lcdNumberClear();
    ui->lbCurCanId->setText(tr("1"));
    for (int index = 0; index < m_btnGroup->buttons().count(); index++) {
        m_btnGroup->button(index)->setStyleSheet("background-color: rgb(14, 221, 111);");

        m_tBtnUnitInfoList[index].m_nodeState  = MOD_NORMAL;
        m_tBtnUnitInfoList[index].m_realLeak   = 0;
        m_tBtnUnitInfoList[index].m_lockLeak   = 0;
        m_tBtnUnitInfoList[index].m_alarmLeak  = 0;
        m_tBtnUnitInfoList[index].m_baseLeak   = 0;
        m_tBtnUnitInfoList[index].m_realTemp   = 0;
        m_tBtnUnitInfoList[index].m_lockTemp   = 0;
        m_tBtnUnitInfoList[index].m_alarmTemp  = 0;
        m_tBtnUnitInfoList[index].m_normalFlag = false;
        m_tBtnUnitInfoList[index].m_alarmFlag  = false;
        m_tBtnUnitInfoList[index].m_errorFlag  = false;
        m_tBtnUnitInfoList[index].m_offLineFlag= false;
    }
    emit sigThreadRestart(m_pass);
}

