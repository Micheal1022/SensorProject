#include "nodeinfo.h"
#include "stateinfo.h"
#include "ui_nodeinfo.h"
#include <QTimer>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QStringList>
#include <QButtonGroup>
#include "SQLite/sqlite.h"
#include <CanBusThread/canbusthread.h>
#define COLUMNSIZE 15
#define PAGESIZE 200
NodeInfo::NodeInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NodeInfo)
{
    ui->setupUi(this);

    initLayout();
    initConnect();
    lcdNumberClear();
}

NodeInfo::~NodeInfo()
{
    delete ui;
}

void NodeInfo::initLayout()
{
    QVBoxLayout *verticalLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    verticalLayout->setSpacing(5);
    verticalLayout->setContentsMargins(5, 0, 5, 0);

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(5);

    m_gridLayout = new QGridLayout();
    m_gridLayout->setHorizontalSpacing(15);
    m_gridLayout->setVerticalSpacing(5);
    m_gridLayout->setContentsMargins(10, 3, 0, 10);

    horizontalLayout->addLayout(m_gridLayout);
    QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);

    verticalLayout->addLayout(horizontalLayout);
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(verticalSpacer);

    ui->scrollArea->verticalScrollBar()->setSingleStep(55*10);
}

void NodeInfo::lcdNumberClear()
{
    ui->lbArea->clear();
    ui->lbNodeType->clear();
    ui->lbNodeCanId->clear();
    ui->lbNodeState->clear();
    ui->lcdNb_AV1->display(0);
    ui->lcdNb_BV1->display(0);
    ui->lcdNb_CV1->display(0);
    ui->lcdNb_AV2->display(0);
    ui->lcdNb_BV2->display(0);
    ui->lcdNb_CV2->display(0);
    ui->lcdNb_AI1->display(0);
    ui->lcdNb_BI1->display(0);
    ui->lcdNb_CI1->display(0);
}

void NodeInfo::showCurNodeValue(int curBtn)
{
    lcdNumberClear();
    ui->lbArea->setText(m_tBtnUnitInfoList.at(curBtn).m_nodeArea);
    ui->lbNodeCanId->setText(QString::number(m_pass)+"-"+QString::number(m_tBtnUnitInfoList.at(curBtn).m_canId));

    int state = m_tBtnUnitInfoList.at(curBtn).m_nodeState;
    switch (state) {
    case GlobalData::NodePowerLost:
        ui->lbNodeState->setText("供电中断");
        break;
    case GlobalData::NodeOverCurrent:
        ui->lbNodeState->setText("节点过流");
        break;
    case GlobalData::NodeLackPhase:
        ui->lbNodeState->setText("节点缺相");
        break;
    case GlobalData::NodeOverVoltage:
        ui->lbNodeState->setText("节点过压");
        break;
    case GlobalData::NodeUnderVoltage:
        ui->lbNodeState->setText("节点欠压");
        break;
    case GlobalData::NodeCanBusError:
        ui->lbNodeState->setText("通讯故障");
        break;
    case GlobalData::NodeNormal:
        ui->lbNodeState->setText("节点正常");
        break;


    }

    int nodeType = m_tBtnUnitInfoList.at(curBtn).m_nodeType;
    nodeValueType(nodeType);
    switch (nodeType) {
    case GlobalData::MODE_V:
        ui->lbNodeType->setText("六路单相电压型");
        break;
    case GlobalData::MODE_V3:
        ui->lbNodeType->setText("双路三相电压型");
        break;
    case GlobalData::MODE_VN3:
        ui->lbNodeType->setText("三相双路有零");
        break;
    case GlobalData::MODE_VA:
        ui->lbNodeType->setText("单相电压电流");
        break;
    case GlobalData::MODE_DVA:
        ui->lbNodeType->setText("直流电压电流");
        break;
    case GlobalData::MODE_VA3:
        ui->lbNodeType->setText("三相电压电流型");
        break;
    case GlobalData::MODE_VAN3:
        ui->lbNodeType->setText("电压电流有零");
        break;
    case GlobalData::MODE_2VAN3:
        ui->lbNodeType->setText("两路三相电压一路三相电流");
        break;
    }

    //    qDebug()<<"*********************";
    //    qDebug()<<"pass : "<<m_pass<<" --  "<<"canId : "<<m_tBtnUnitInfoList.at(curBtn).m_canId;
    //    qDebug()<<"m_valueAV_1 : "<<m_tBtnUnitInfoList[curBtn].m_valueAV_1;
    //    qDebug()<<"m_valueBV_1 : "<<m_tBtnUnitInfoList[curBtn].m_valueAV_1;
    //    qDebug()<<"m_valueCV_1 : "<<m_tBtnUnitInfoList[curBtn].m_valueAV_1;

    //    qDebug()<<"m_valueAV_2 : "<<m_tBtnUnitInfoList[curBtn].m_valueAV_2;
    //    qDebug()<<"m_valueCV_2 : "<<m_tBtnUnitInfoList[curBtn].m_valueAV_2;
    //    qDebug()<<"m_valueCV_2 : "<<m_tBtnUnitInfoList[curBtn].m_valueAV_2;

    //    qDebug()<<"m_valueAI_1 : "<<m_tBtnUnitInfoList[curBtn].m_valueAI_1;
    //    qDebug()<<"m_valueBI_1 : "<<m_tBtnUnitInfoList[curBtn].m_valueAI_1;
    //    qDebug()<<"m_valueCI_1 : "<<m_tBtnUnitInfoList[curBtn].m_valueAI_1;

    switch (nodeType) {
    case GlobalData::MODE_DVA:
        ui->lcdNb_AV1->display(m_tBtnUnitInfoList[curBtn].m_valueAV_1);
        ui->lcdNb_AI1->display(m_tBtnUnitInfoList[curBtn].m_valueAI_1);
        break;
    case GlobalData::MODE_V:
    case GlobalData::MODE_V3:
    case GlobalData::MODE_VN3:
        ui->lcdNb_AV1->display(m_tBtnUnitInfoList[curBtn].m_valueAV_1);
        ui->lcdNb_BV1->display(m_tBtnUnitInfoList[curBtn].m_valueBV_1);
        ui->lcdNb_CV1->display(m_tBtnUnitInfoList[curBtn].m_valueCV_1);

        ui->lcdNb_AV2->display(m_tBtnUnitInfoList[curBtn].m_valueAV_2);
        ui->lcdNb_BV2->display(m_tBtnUnitInfoList[curBtn].m_valueBV_2);
        ui->lcdNb_CV2->display(m_tBtnUnitInfoList[curBtn].m_valueCV_2);
        break;
    case GlobalData::MODE_VA:
    case GlobalData::MODE_VA3:
    case GlobalData::MODE_VAN3:
        ui->lcdNb_AV1->display(m_tBtnUnitInfoList[curBtn].m_valueAV_1);
        ui->lcdNb_BV1->display(m_tBtnUnitInfoList[curBtn].m_valueBV_1);
        ui->lcdNb_CV1->display(m_tBtnUnitInfoList[curBtn].m_valueCV_1);

        ui->lcdNb_AI1->display(m_tBtnUnitInfoList[curBtn].m_valueAI_1);
        ui->lcdNb_BI1->display(m_tBtnUnitInfoList[curBtn].m_valueBI_1);
        ui->lcdNb_CI1->display(m_tBtnUnitInfoList[curBtn].m_valueCI_1);
        break;
    case GlobalData::MODE_2VAN3:
        ui->lcdNb_AV1->display(m_tBtnUnitInfoList[curBtn].m_valueAV_1);
        ui->lcdNb_BV1->display(m_tBtnUnitInfoList[curBtn].m_valueBV_1);
        ui->lcdNb_CV1->display(m_tBtnUnitInfoList[curBtn].m_valueCV_1);

        ui->lcdNb_AV2->display(m_tBtnUnitInfoList[curBtn].m_valueAV_2);
        ui->lcdNb_BV2->display(m_tBtnUnitInfoList[curBtn].m_valueBV_2);
        ui->lcdNb_CV2->display(m_tBtnUnitInfoList[curBtn].m_valueCV_2);

        ui->lcdNb_AI1->display(m_tBtnUnitInfoList[curBtn].m_valueAI_1);
        ui->lcdNb_BI1->display(m_tBtnUnitInfoList[curBtn].m_valueBI_1);
        ui->lcdNb_CI1->display(m_tBtnUnitInfoList[curBtn].m_valueCI_1);
        break;
    }


}

void NodeInfo::nodeConf(int pass,QList<int> nodeList)
{
    m_curBtn = 0;
    m_pass = pass;
    m_currentPage = 0;
    m_nodeCount = nodeList.count();
    m_pageCount = getPageCount(nodeList) - 1;
    m_nodeErrorList.clear();
    m_powerLostList.clear();
    if(m_pageCount > 0)
    {
        ui->tBtnHeadPage->setEnabled(false);
        ui->tBtnLastPage->setEnabled(false);
        ui->tBtnTailPage->setEnabled(true);
        ui->tBtnNestPage->setEnabled(true);
    }
    else
    {
        ui->tBtnHeadPage->setEnabled(false);
        ui->tBtnTailPage->setEnabled(false);
        ui->tBtnNestPage->setEnabled(false);
        ui->tBtnLastPage->setEnabled(false);
    }
    ui->lbNodeCount->setText(QString::number(nodeList.count()));

    initNodeBtn(nodeList);

}

void NodeInfo::initNodeBtn(QList<int> nodeList)
{

    m_tBtnUnitInfoList.clear();
    for(int index = 0;index < nodeList.count();index++)
    {
        QToolButton *tBtn = new QToolButton(ui->scrollAreaWidgetContents);
        tBtn->setFocusPolicy(Qt::ClickFocus);
        tBtn->setMinimumSize(QSize(50, 45));
        tBtn->setMaximumSize(QSize(50, 45));

        QString canIdStr = QString::number(nodeList.at(index));
        QSqlDatabase db = SQLite::openConnection();
        QStringList nodeInfo = SQLite::getNodeInfo(db,QString::number(m_pass),canIdStr);
        SQLite::closeConnection(db);

        BtnUnitInfo btnUnitInfo;
        btnUnitInfo.initData();
        QString ratioStr = nodeInfo.value(0);
        btnUnitInfo.m_ratio    = ratioStr.toInt();
        btnUnitInfo.m_nodeArea = nodeInfo.value(1);
        m_tBtnUnitInfoList.append(btnUnitInfo);
        tBtn->setText(QString::number(nodeList.at(index))+tr("\n未注册"));
        m_gridLayout->addWidget(tBtn, index/COLUMNSIZE, index%COLUMNSIZE, 1, 1);
        m_tBtnGroup->addButton(tBtn,index);
    }
}

int NodeInfo::getPageCount(QList<int> nodeList)
{
    int rowCount = nodeList.size() / 15;
    if(nodeList.size() % 15 > 0)
    {
        rowCount += 1;
    }
    int pageCount = rowCount / 9;
    if(pageCount % 9)
        pageCount += 1;


    return pageCount;
}

QStringList NodeInfo::addStringList(int pass, int canId, int state, QString alarmTime, QString area)
{
    QString passStr = QString::number(pass);
    QString canIdStr= QString::number(canId);
    QString stateStr;
    switch (state) {
    case GlobalData::NodePowerLost:
        stateStr = QString(tr("供电中断"));
        break;
    case GlobalData::NodeOverCurrent:
        stateStr = QString(tr("节点过流"));
        break;
    case GlobalData::NodeLackPhase:
        stateStr = QString(tr("节点缺相"));
        break;
    case GlobalData::NodeOverVoltage:
        stateStr = QString(tr("节点过压"));
        break;
    case GlobalData::NodeUnderVoltage:
        stateStr = QString(tr("节点欠压"));
        break;
    case GlobalData::NodeCanBusError:
        stateStr = QString(tr("通讯故障"));
        break;
    }

    QStringList strList;
    strList<<passStr<<canIdStr<<stateStr<<alarmTime<<area;

    return strList;
}

void NodeInfo::delStringList(QList<QStringList> &stringList, int pass, int canId, int state)
{
    QString passStr = QString::number(pass);
    QString canIdStr= QString::number(canId);
    QString stateStr;

    switch (state) {
    case GlobalData::NodePowerLost:
        stateStr = QString(tr("供电中断"));
        break;
    case GlobalData::NodeOverCurrent:
        stateStr = QString(tr("节点过流"));
        break;
    case GlobalData::NodeLackPhase:
        stateStr = QString(tr("节点缺相"));
        break;
    case GlobalData::NodeOverVoltage:
        stateStr = QString(tr("节点过压"));
        break;
    case GlobalData::NodeUnderVoltage:
        stateStr = QString(tr("节点欠压"));
        break;
    case GlobalData::NodeCanBusError:
        stateStr = QString(tr("通讯故障"));
        break;
    default:
        break;
    }

    for(int index = 0;index < stringList.count();index++)
    {
        if(stringList.at(index).at(0) == passStr&&stringList.at(index).at(1) ==canIdStr && stringList.at(index).at(2) == stateStr)
        {
            stringList.removeAt(index);
        }
    }
}

void NodeInfo::initConnect()
{
    m_timer = new QTimer;
    connect(m_timer,&QTimer::timeout,this,&NodeInfo::slotTimeOut);
    m_timer->start(1500);

    m_tBtnGroup = new QButtonGroup;
    connect(m_tBtnGroup,SIGNAL(buttonClicked(int)),this,SLOT(slotBtnGroup(int)));
    connect(ui->tBtnPowerInfo,&QToolButton::clicked,this,&NodeInfo::slotBtnPowerInfo);
    connect(ui->tBtnErrorInfo,&QToolButton::clicked,this,&NodeInfo::slotBtnErrorInfo);
    connect(this,SIGNAL(sigSetValue(int)),ui->scrollArea->verticalScrollBar(),SLOT(setValue(int)));
    connect(ui->tBtnHeadPage,&QToolButton::clicked,this,&NodeInfo::slotBtnHeadPage);
    connect(ui->tBtnTailPage,&QToolButton::clicked,this,&NodeInfo::slotBtnTailPage);
    connect(ui->tBtnLastPage,&QToolButton::clicked,this,&NodeInfo::slotBtnLastPage);
    connect(ui->tBtnNestPage,&QToolButton::clicked,this,&NodeInfo::slotBtnNestPage);
}

QString NodeInfo::btnSetText(int nodeType)
{
    QString btnText;
    switch (nodeType) {
    case 2:
        btnText = tr("V3");
        break;
    case 3:
        btnText = tr("V");
        break;
    case 4:
        btnText = tr("VA3");
        break;
    case 5:
        btnText = tr("DVA");
        break;
    case 6:
        btnText = tr("VA");
        break;
    case 7:
        btnText = tr("VN3");
        break;
    case 8:
        btnText = tr("VAN3");
        break;
    case 9:
        btnText = tr("2VAN3");
        break;
    }
    return btnText;
}

void NodeInfo::nodeValueType(int nodeType)
{
    switch (nodeType)
    {
    case GlobalData::MODE_DVA:
        //AV-1                                  //AI-1                                //AV-2
        ui->lbVA_1->setText(tr("直流电压："));    ui->lbIA_1->setText(tr("直流电流："));  ui->lbVA_2->setText(tr(""));
        ui->lbVA_1->setVisible(true);           ui->lbIA_1->setVisible(true);         ui->lbVA_2->setVisible(false);
        ui->lcdNb_AV1->setVisible(true);        ui->lcdNb_AI1->setVisible(true);      ui->lcdNb_AV2->setVisible(false);
        ui->lbUnitVA_1->setVisible(true);       ui->lbUnitIA_1->setVisible(true);     ui->lbUnitVA_2->setVisible(false);
        //BV-1                                  //BI-1                                //BV-2
        ui->lbVB_1->setText(tr(""));            ui->lbIB_1->setText(tr(""));          ui->lbVB_2->setText(tr(""));
        ui->lbVB_1->setVisible(false);          ui->lbIB_1->setVisible(false);        ui->lbVB_2->setVisible(false);
        ui->lcdNb_BV1->setVisible(false);       ui->lcdNb_BI1->setVisible(false);     ui->lcdNb_BV2->setVisible(false);
        ui->lbUnitVB_1->setVisible(false);      ui->lbUnitIB_1->setVisible(false);    ui->lbUnitVB_2->setVisible(false);
        //CV-1                                  //CI-1                                //CV-2
        ui->lbVC_1->setText(tr(""));            ui->lbIC_1->setText(tr(""));          ui->lbVC_2->setText(tr(""));
        ui->lbVC_1->setVisible(false);          ui->lbIC_1->setVisible(false);        ui->lbVC_2->setVisible(false);
        ui->lcdNb_CV1->setVisible(false);       ui->lcdNb_CI1->setVisible(false);     ui->lcdNb_CV2->setVisible(false);
        ui->lbUnitVC_1->setVisible(false);      ui->lbUnitIC_1->setVisible(false);    ui->lbUnitVC_2->setVisible(false);

        break;
    case GlobalData::MODE_V:
        //AV-1                                  //AI-1                                //AV-2
        ui->lbVA_1->setText(tr("第一路电压："));  ui->lbIA_1->setText(tr(""));          ui->lbVA_2->setText(tr("第四路电压："));
        ui->lbVA_1->setVisible(true);           ui->lbIA_1->setVisible(false);        ui->lbVA_2->setVisible(true);
        ui->lcdNb_AV1->setVisible(true);        ui->lcdNb_AI1->setVisible(false);     ui->lcdNb_AV2->setVisible(true);
        ui->lbUnitVA_1->setVisible(true);       ui->lbUnitIA_1->setVisible(false);    ui->lbUnitVA_2->setVisible(true);
        //BV-1                                  //BI-1                                //BV-2
        ui->lbVB_1->setText(tr("第二路电压："));  ui->lbIB_1->setText(tr(""));          ui->lbVB_2->setText(tr("第五路电压："));
        ui->lbVB_1->setVisible(true);           ui->lbIB_1->setVisible(false);        ui->lbVB_2->setVisible(true);
        ui->lcdNb_BV1->setVisible(true);        ui->lcdNb_BI1->setVisible(false);     ui->lcdNb_BV2->setVisible(true);
        ui->lbUnitVB_1->setVisible(true);       ui->lbUnitIB_1->setVisible(false);    ui->lbUnitVB_2->setVisible(true);
        //CV-1                                  //CI-1                                //CV-2
        ui->lbVC_1->setText(tr("第三路电压："));  ui->lbIC_1->setText(tr(""));          ui->lbVC_2->setText(tr("第六路电压："));
        ui->lbVC_1->setVisible(true);           ui->lbIC_1->setVisible(false);        ui->lbVC_2->setVisible(true);
        ui->lcdNb_CV1->setVisible(true);        ui->lcdNb_CI1->setVisible(false);     ui->lcdNb_CV2->setVisible(true);
        ui->lbUnitVC_1->setVisible(true);       ui->lbUnitIC_1->setVisible(false);    ui->lbUnitVC_2->setVisible(true);

        break;
    case GlobalData::MODE_V3:
    case GlobalData::MODE_VN3:
        //AV-1                                   //AI-1                                //AV-2
        ui->lbVA_1->setText(tr("一路A相电压："));  ui->lbIA_1->setText(tr(""));          ui->lbVA_2->setText(tr("二路A相电压："));
        ui->lbVA_1->setVisible(true);            ui->lbIA_1->setVisible(false);        ui->lbVA_2->setVisible(true);
        ui->lcdNb_AV1->setVisible(true);         ui->lcdNb_AI1->setVisible(false);     ui->lcdNb_AV2->setVisible(true);
        ui->lbUnitVA_1->setVisible(true);        ui->lbUnitIA_1->setVisible(false);    ui->lbUnitVA_2->setVisible(true);
        //BV-1                                           //BI-1                                //BV-2
        ui->lbVB_1->setText(tr("一路B相电压："));  ui->lbIB_1->setText(tr(""));          ui->lbVB_2->setText(tr("二路B相电压："));
        ui->lbVB_1->setVisible(true);            ui->lbIB_1->setVisible(false);        ui->lbVB_2->setVisible(true);
        ui->lcdNb_BV1->setVisible(true);         ui->lcdNb_BI1->setVisible(false);     ui->lcdNb_BV2->setVisible(true);
        ui->lbUnitVB_1->setVisible(true);        ui->lbUnitIB_1->setVisible(false);    ui->lbUnitVB_2->setVisible(true);
        //CV-1                                   //CI-1                                //CV-2
        ui->lbVC_1->setText(tr("一路C相电压："));  ui->lbIC_1->setText(tr(""));          ui->lbVC_2->setText(tr("二路C相电压："));
        ui->lbVC_1->setVisible(true);            ui->lbIC_1->setVisible(false);        ui->lbVC_2->setVisible(true);
        ui->lcdNb_CV1->setVisible(true);         ui->lcdNb_CI1->setVisible(false);     ui->lcdNb_CV2->setVisible(true);
        ui->lbUnitVC_1->setVisible(true);        ui->lbUnitIC_1->setVisible(false);    ui->lbUnitVC_2->setVisible(true);

        break;
    case GlobalData::MODE_VA:
        //AV-1                                  //AI-1                                  //AV-2
        ui->lbVA_1->setText(tr("第一路电压："));  ui->lbIA_1->setText(tr("第一路电流："));  ui->lbVA_2->setText(tr(""));
        ui->lbVA_1->setVisible(true);           ui->lbIA_1->setVisible(true);           ui->lbVA_2->setVisible(false);
        ui->lcdNb_AV1->setVisible(true);        ui->lcdNb_AI1->setVisible(true);        ui->lcdNb_AV2->setVisible(false);
        ui->lbUnitVA_1->setVisible(true);       ui->lbUnitIA_1->setVisible(true);       ui->lbUnitVA_2->setVisible(false);
        //BV-1                                  //BI-1                                  //BV-2
        ui->lbVB_1->setText(tr("第二路电压："));  ui->lbIB_1->setText(tr("第二路电流："));  ui->lbVB_2->setText(tr(""));
        ui->lbVB_1->setVisible(true);           ui->lbIB_1->setVisible(true);           ui->lbVB_2->setVisible(false);
        ui->lcdNb_BV1->setVisible(true);        ui->lcdNb_BI1->setVisible(true);        ui->lcdNb_BV2->setVisible(false);
        ui->lbUnitVB_1->setVisible(true);       ui->lbUnitIB_1->setVisible(true);       ui->lbUnitVB_2->setVisible(false);
        //CV-1                                  //CI-1                                  //CV-2
        ui->lbVC_1->setText(tr("第三路电压："));  ui->lbIC_1->setText(tr("第三路电流："));  ui->lbVC_2->setText(tr(""));
        ui->lbVC_1->setVisible(true);           ui->lbIC_1->setVisible(true);           ui->lbVC_2->setVisible(false);
        ui->lcdNb_CV1->setVisible(true);        ui->lcdNb_CI1->setVisible(true);        ui->lcdNb_CV2->setVisible(false);
        ui->lbUnitVC_1->setVisible(true);       ui->lbUnitIC_1->setVisible(true);       ui->lbUnitVC_2->setVisible(false);
        break;
    case GlobalData::MODE_VA3:
    case GlobalData::MODE_VAN3:
        //AV-1                                   //AI-1                                   //AV-2
        ui->lbVA_1->setText(tr("一路A相电压："));  ui->lbIA_1->setText(tr("一路A相电流："));  ui->lbVA_2->setText(tr(""));
        ui->lbVA_1->setVisible(true);            ui->lbIA_1->setVisible(true);           ui->lbVA_2->setVisible(false);
        ui->lcdNb_AV1->setVisible(true);         ui->lcdNb_AI1->setVisible(true);        ui->lcdNb_AV2->setVisible(false);
        ui->lbUnitVA_1->setVisible(true);        ui->lbUnitIA_1->setVisible(true);        ui->lbUnitVA_2->setVisible(false);
        //BV-1                                   //BI-1                                   //BV-2
        ui->lbVB_1->setText(tr("一路B相电压："));  ui->lbIB_1->setText(tr("一路B相电流："));  ui->lbVB_2->setText(tr(""));
        ui->lbVB_1->setVisible(true);            ui->lbIB_1->setVisible(true);           ui->lbVB_2->setVisible(false);
        ui->lcdNb_BV1->setVisible(true);         ui->lcdNb_BI1->setVisible(true);        ui->lcdNb_BV2->setVisible(false);
        ui->lbUnitVB_1->setVisible(true);        ui->lbUnitIB_1->setVisible(true);       ui->lbUnitVB_2->setVisible(false);
        //CI-1                                   //CI-1                                   //CV-2
        ui->lbVC_1->setText(tr("一路C相电压："));  ui->lbIC_1->setText(tr("一路C相电流："));  ui->lbVC_2->setText(tr(""));
        ui->lbVC_1->setVisible(true);            ui->lbIC_1->setVisible(true);           ui->lbVC_2->setVisible(false);
        ui->lcdNb_CV1->setVisible(true);         ui->lcdNb_CI1->setVisible(true);        ui->lcdNb_CV2->setVisible(false);
        ui->lbUnitVC_1->setVisible(true);        ui->lbUnitIC_1->setVisible(true);       ui->lbUnitVC_2->setVisible(false);

        break;
    case GlobalData::MODE_2VAN3:
        //AV-1                                   //AI-1                                   //AV-2
        ui->lbVA_1->setText(tr("一路A相电压："));  ui->lbIA_1->setText(tr("一路A相电流："));  ui->lbVA_2->setText(tr("二路A相电压："));
        ui->lbVA_1->setVisible(true);            ui->lbIA_1->setVisible(true);           ui->lbVA_2->setVisible(true);
        ui->lcdNb_AV1->setVisible(true);         ui->lcdNb_AI1->setVisible(true);        ui->lcdNb_AV2->setVisible(true);
        ui->lbUnitVA_1->setVisible(true);        ui->lbUnitIA_1->setVisible(true);        ui->lbUnitVA_2->setVisible(true);
        //BV-1                                   //BI-1                                   //BV-2
        ui->lbVB_1->setText(tr("一路B相电压："));  ui->lbIB_1->setText(tr("一路B相电流："));  ui->lbVB_2->setText(tr("二路B相电压："));
        ui->lbVB_1->setVisible(true);            ui->lbIB_1->setVisible(true);           ui->lbVB_2->setVisible(true);
        ui->lcdNb_BV1->setVisible(true);         ui->lcdNb_BI1->setVisible(true);        ui->lcdNb_BV2->setVisible(true);
        ui->lbUnitVB_1->setVisible(true);        ui->lbUnitIB_1->setVisible(true);       ui->lbUnitVB_2->setVisible(true);
        //CI-1                                   //CI-1                                   //CV-2
        ui->lbVC_1->setText(tr("一路C相电压："));  ui->lbIC_1->setText(tr("一路C相电流："));  ui->lbVC_2->setText(tr("二路C相电压："));
        ui->lbVC_1->setVisible(true);            ui->lbIC_1->setVisible(true);           ui->lbVC_2->setVisible(true);
        ui->lcdNb_CV1->setVisible(true);         ui->lcdNb_CI1->setVisible(true);        ui->lcdNb_CV2->setVisible(true);
        ui->lbUnitVC_1->setVisible(true);        ui->lbUnitIC_1->setVisible(true);       ui->lbUnitVC_2->setVisible(true);
        break;
    }
}

void NodeInfo::updateNodeState(int index,int canId)
{
    if (m_tBtnUnitInfoList[index].m_powerLost == true) {
        m_tBtnUnitInfoList[index].m_powerLost   = false;
        delStringList(m_powerLostList,m_pass,canId,GlobalData::NodePowerLost);
    } else if (m_tBtnUnitInfoList[index].m_lackPhase == true) {
        m_tBtnUnitInfoList[index].m_lackPhase   = false;
        delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeLackPhase);
    } else if (m_tBtnUnitInfoList[index].m_overCurrent == true) {
        m_tBtnUnitInfoList[index].m_overCurrent  = false;
        delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeOverCurrent);
    } else if (m_tBtnUnitInfoList[index].m_overVoltage == true) {
        m_tBtnUnitInfoList[index].m_overVoltage  = false;
        delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeOverVoltage);
    } else if (m_tBtnUnitInfoList[index].m_underVoltage == true) {
        m_tBtnUnitInfoList[index].m_underVoltage = false;
        delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeUnderVoltage);
    } else if (m_tBtnUnitInfoList[index].m_canBusError == true) {
        m_tBtnUnitInfoList[index].m_canBusError  = false;
        delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeCanBusError);
    }
}

void NodeInfo::slotBtnPowerInfo()
{
    StateInfo stateInfo(this);
    stateInfo.initConf(m_powerLostList, StateInfo::PowerData);
    stateInfo.exec();
}

void NodeInfo::slotBtnErrorInfo()
{
    StateInfo stateInfo(this);
    stateInfo.initConf(m_nodeErrorList, StateInfo::ErrorData);
    stateInfo.exec();
}

void NodeInfo::slotBtnGroup(int curBtn)
{
    m_curBtn = curBtn;
    showCurNodeValue(curBtn);
}

void NodeInfo::slotTimeOut()
{
    showCurNodeValue(m_curBtn);
    int errorNum = m_powerLostList.size() + m_nodeErrorList.size();
    emit sigErrorCount(errorNum);

}

void NodeInfo::slotBtnHeadPage()
{
    m_currentPage = 0;
    emit sigSetValue(0);
    ui->tBtnLastPage->setEnabled(false);
    ui->tBtnNestPage->setEnabled(true);
    ui->tBtnHeadPage->setEnabled(false);
    ui->tBtnTailPage->setEnabled(true);
}

void NodeInfo::slotBtnTailPage()
{
    m_currentPage = m_pageCount;
    emit sigSetValue(m_currentPage*PAGESIZE);
    ui->tBtnNestPage->setEnabled(false);
    ui->tBtnLastPage->setEnabled(true);
    ui->tBtnTailPage->setEnabled(false);
    ui->tBtnHeadPage->setEnabled(true);
}

void NodeInfo::slotBtnLastPage()
{
    m_currentPage--;
    if(m_currentPage > 0)
    {
        emit sigSetValue(m_currentPage*PAGESIZE);
        ui->tBtnLastPage->setEnabled(true);
        ui->tBtnHeadPage->setEnabled(true);
        ui->tBtnNestPage->setEnabled(true);
        ui->tBtnTailPage->setEnabled(true);
    }
    else if(m_currentPage == 0)
    {
        emit sigSetValue(m_currentPage*PAGESIZE);
        ui->tBtnLastPage->setEnabled(false);
        ui->tBtnHeadPage->setEnabled(false);
        ui->tBtnNestPage->setEnabled(true);
        ui->tBtnTailPage->setEnabled(true);
    }
}

void NodeInfo::slotBtnNestPage()
{
    m_currentPage++;
    if(m_currentPage < m_pageCount)
    {
        emit sigSetValue(m_currentPage*PAGESIZE);
        ui->tBtnLastPage->setEnabled(true);
        ui->tBtnHeadPage->setEnabled(true);
        ui->tBtnNestPage->setEnabled(true);
        ui->tBtnTailPage->setEnabled(true);
    }
    else if(m_currentPage == m_pageCount)
    {
        emit sigSetValue(m_currentPage*PAGESIZE);
        ui->tBtnNestPage->setEnabled(false);
        ui->tBtnTailPage->setEnabled(false);
        ui->tBtnLastPage->setEnabled(true);
        ui->tBtnHeadPage->setEnabled(true);
    }
}

void NodeInfo::slotCurCanId(int curCanId)
{
    ui->lbCurCanId->setText(QString::number(curCanId));
}


void NodeInfo::slotRecvCanData(int index, int pass, int canId, int type, int state, QList<int> voltageList, QList<qreal> currentList)
{

    if(m_pass == pass)
    {
        QString nodeArea = m_tBtnUnitInfoList[index].m_nodeArea;
        m_tBtnUnitInfoList[index].m_canId = canId;
        m_tBtnUnitInfoList[index].m_nodeType = type;
        m_tBtnUnitInfoList[index].m_nodeState= state;

        ui->lbCurCanId->setText(QString::number(canId));

        //报警时间
        QDateTime currentTime = QDateTime::currentDateTime();
        QString alarmTime = currentTime.toString(tr("yyyy年MM月dd日 hh:mm:ss"));

        if(state != GlobalData::NodeCanBusError)
        {
            QString btnText = m_tBtnGroup->button(index)->text();
            int ind = btnText.indexOf("\n");
            btnText = btnText.left(ind+1)+btnSetText(type);
            m_tBtnGroup->button(index)->setText(btnText);
        }

        switch (type)
        {
        case GlobalData::MODE_DVA:
            m_tBtnUnitInfoList[index].m_valueAV_1 = QString::number(voltageList.value(0));
            m_tBtnUnitInfoList[index].m_valueAI_1 = QString::number(currentList.value(0) * m_tBtnUnitInfoList[index].m_ratio);
            break;
        case GlobalData::MODE_V:
        case GlobalData::MODE_V3:
        case GlobalData::MODE_VN3:

            m_tBtnUnitInfoList[index].m_valueAV_1 = QString::number(voltageList.value(0));
            m_tBtnUnitInfoList[index].m_valueBV_1 = QString::number(voltageList.value(1));
            m_tBtnUnitInfoList[index].m_valueCV_1 = QString::number(voltageList.value(2));

            m_tBtnUnitInfoList[index].m_valueAV_2 = QString::number(voltageList.value(3));
            m_tBtnUnitInfoList[index].m_valueBV_2 = QString::number(voltageList.value(4));
            m_tBtnUnitInfoList[index].m_valueCV_2 = QString::number(voltageList.value(5));

            break;
        case GlobalData::MODE_VA:
        case GlobalData::MODE_VA3:
        case GlobalData::MODE_VAN3:

            m_tBtnUnitInfoList[index].m_valueAV_1 = QString::number(voltageList.value(0));
            m_tBtnUnitInfoList[index].m_valueBV_1 = QString::number(voltageList.value(1));
            m_tBtnUnitInfoList[index].m_valueCV_1 = QString::number(voltageList.value(2));

            m_tBtnUnitInfoList[index].m_valueAI_1 = QString::number(currentList.value(0) * m_tBtnUnitInfoList[index].m_ratio);
            m_tBtnUnitInfoList[index].m_valueBI_1 = QString::number(currentList.value(1) * m_tBtnUnitInfoList[index].m_ratio);
            m_tBtnUnitInfoList[index].m_valueCI_1 = QString::number(currentList.value(2) * m_tBtnUnitInfoList[index].m_ratio);

            break;
        case GlobalData::MODE_2VAN3:

            m_tBtnUnitInfoList[index].m_valueAV_1 = QString::number(voltageList.value(0));
            m_tBtnUnitInfoList[index].m_valueBV_1 = QString::number(voltageList.value(1));
            m_tBtnUnitInfoList[index].m_valueCV_1 = QString::number(voltageList.value(2));

            m_tBtnUnitInfoList[index].m_valueAV_2 = QString::number(voltageList.value(3));
            m_tBtnUnitInfoList[index].m_valueBV_2 = QString::number(voltageList.value(4));
            m_tBtnUnitInfoList[index].m_valueCV_2 = QString::number(voltageList.value(5));

            m_tBtnUnitInfoList[index].m_valueAI_1 = QString::number(currentList.value(0) * m_tBtnUnitInfoList[index].m_ratio);
            m_tBtnUnitInfoList[index].m_valueBI_1 = QString::number(currentList.value(1) * m_tBtnUnitInfoList[index].m_ratio);
            m_tBtnUnitInfoList[index].m_valueCI_1 = QString::number(currentList.value(2) * m_tBtnUnitInfoList[index].m_ratio);
            break;
        }


        switch (state)
        {
        case GlobalData::NodeNormal://绿色
            m_tBtnGroup->button(index)->setStyleSheet("background-color: rgb(0, 255, 0);");
            updateNodeState(index,canId);
            break;
        case GlobalData::NodeOverCurrent://黄色
            if (m_tBtnUnitInfoList[index].m_overCurrent  == false) {
                m_tBtnUnitInfoList[index].m_overCurrent = true;

                m_tBtnUnitInfoList[index].m_canBusError = false;
                m_tBtnUnitInfoList[index].m_powerLost = false;
                m_tBtnUnitInfoList[index].m_lackPhase = false;
                m_tBtnUnitInfoList[index].m_overVoltage = false;
                m_tBtnUnitInfoList[index].m_underVoltage = false;
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeLackPhase);
                delStringList(m_powerLostList,m_pass,canId,GlobalData::NodePowerLost);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeOverVoltage);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeUnderVoltage);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeCanBusError);

                m_tBtnGroup->button(index)->setStyleSheet("background-color: rgb(255, 175, 0);");
                QStringList nodeErrorList = addStringList(m_pass,canId,state,alarmTime,nodeArea);
                m_nodeErrorList.append(nodeErrorList);
                emit sigSoundOn();

                QSqlDatabase db = SQLite::openConnection();
                SQLite::insertRecord(db,m_pass,canId,state,currentTime.toTime_t(),nodeArea);
                SQLite::closeConnection(db);
            }
            break;
        case GlobalData::NodeLackPhase://黄色
            if (m_tBtnUnitInfoList[index].m_lackPhase == false) {
                m_tBtnUnitInfoList[index].m_lackPhase = true;

                m_tBtnUnitInfoList[index].m_canBusError = false;
                m_tBtnUnitInfoList[index].m_powerLost = false;
                m_tBtnUnitInfoList[index].m_overCurrent = false;
                m_tBtnUnitInfoList[index].m_overVoltage = false;
                m_tBtnUnitInfoList[index].m_underVoltage = false;
                delStringList(m_powerLostList,m_pass,canId,GlobalData::NodePowerLost);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeOverCurrent);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeOverVoltage);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeUnderVoltage);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeCanBusError);

                m_tBtnGroup->button(index)->setStyleSheet("background-color: rgb(255, 175, 0);");
                QStringList nodeErrorList = addStringList(m_pass,canId,state,alarmTime,nodeArea);
                m_nodeErrorList.append(nodeErrorList);
                emit sigSoundOn();

                QSqlDatabase db = SQLite::openConnection();
                SQLite::insertRecord(db,m_pass,canId,state,currentTime.toTime_t(),nodeArea);
                SQLite::closeConnection(db);
            }
            break;
        case GlobalData::NodePowerLost://红色
            if (m_tBtnUnitInfoList[index].m_powerLost == false) {
                m_tBtnUnitInfoList[index].m_powerLost = true;

                m_tBtnUnitInfoList[index].m_canBusError = false;
                m_tBtnUnitInfoList[index].m_lackPhase = false;
                m_tBtnUnitInfoList[index].m_overCurrent = false;
                m_tBtnUnitInfoList[index].m_overVoltage = false;
                m_tBtnUnitInfoList[index].m_underVoltage = false;

                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeLackPhase);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeOverCurrent);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeOverVoltage);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeUnderVoltage);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeCanBusError);

                m_tBtnGroup->button(index)->setStyleSheet("background-color: rgb(255, 0, 0);");
                QStringList pwerLostList = addStringList(m_pass,canId,state,alarmTime,nodeArea);
                m_powerLostList.append(pwerLostList);
                emit sigSoundOn();

                QSqlDatabase db = SQLite::openConnection();
                SQLite::insertRecord(db,m_pass,canId,state,currentTime.toTime_t(),nodeArea);
                SQLite::closeConnection(db);
            }
            break;
        case GlobalData::NodeOverVoltage://黄色
            if (m_tBtnUnitInfoList[index].m_overVoltage  == false) {
                m_tBtnUnitInfoList[index].m_overVoltage = true;

                m_tBtnUnitInfoList[index].m_canBusError = false;
                m_tBtnUnitInfoList[index].m_powerLost = false;
                m_tBtnUnitInfoList[index].m_lackPhase = false;
                m_tBtnUnitInfoList[index].m_overCurrent = false;
                m_tBtnUnitInfoList[index].m_underVoltage = false;

                delStringList(m_powerLostList,m_pass,canId,GlobalData::NodePowerLost);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeLackPhase);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeOverCurrent);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeUnderVoltage);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeCanBusError);

                m_tBtnGroup->button(index)->setStyleSheet("background-color: rgb(255, 175, 0);");
                QStringList nodeErrorList = addStringList(m_pass,canId,state,alarmTime,nodeArea);
                m_nodeErrorList.append(nodeErrorList);
                emit sigSoundOn();

                QSqlDatabase db = SQLite::openConnection();
                SQLite::insertRecord(db,m_pass,canId,state,currentTime.toTime_t(),nodeArea);
                SQLite::closeConnection(db);
            }
            break;
        case GlobalData::NodeUnderVoltage://黄色
            if (m_tBtnUnitInfoList[index].m_underVoltage == false) {
                m_tBtnUnitInfoList[index].m_underVoltage = true;

                m_tBtnUnitInfoList[index].m_canBusError = false;
                m_tBtnUnitInfoList[index].m_powerLost = false;
                m_tBtnUnitInfoList[index].m_lackPhase = false;
                m_tBtnUnitInfoList[index].m_overCurrent = false;
                m_tBtnUnitInfoList[index].m_overVoltage = false;

                delStringList(m_powerLostList,m_pass,canId,GlobalData::NodePowerLost);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeLackPhase);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeOverCurrent);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeOverVoltage);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeCanBusError);

                m_tBtnGroup->button(index)->setStyleSheet("background-color: rgb(255, 175, 0);");
                QStringList nodeErrorList = addStringList(m_pass,canId,state,alarmTime,nodeArea);
                m_nodeErrorList.append(nodeErrorList);
                emit sigSoundOn();

                QSqlDatabase db = SQLite::openConnection();
                SQLite::insertRecord(db,m_pass,canId,state,currentTime.toTime_t(),nodeArea);
                SQLite::closeConnection(db);
            }
            break;
        case GlobalData::NodeCanBusError://黄色
            if (m_tBtnUnitInfoList[index].m_canBusError == false) {
                m_tBtnUnitInfoList[index].m_canBusError = true;

                m_tBtnUnitInfoList[index].m_powerLost = false;
                m_tBtnUnitInfoList[index].m_lackPhase = false;
                m_tBtnUnitInfoList[index].m_overCurrent = false;
                m_tBtnUnitInfoList[index].m_overVoltage = false;
                m_tBtnUnitInfoList[index].m_underVoltage = false;

                delStringList(m_powerLostList,m_pass,canId,GlobalData::NodePowerLost);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeLackPhase);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeOverCurrent);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeOverVoltage);
                delStringList(m_nodeErrorList,m_pass,canId,GlobalData::NodeUnderVoltage);

                m_tBtnGroup->button(index)->setStyleSheet("background-color: rgb(171, 171, 171);");
                QStringList nodeErrorList = addStringList(m_pass,canId,state,alarmTime,nodeArea);
                m_nodeErrorList.append(nodeErrorList);
                emit sigSoundOn();

                QSqlDatabase db = SQLite::openConnection();
                SQLite::insertRecord(db,m_pass,canId,state,currentTime.toTime_t(),nodeArea);
                SQLite::closeConnection(db);
            }
            break;
        }
    }
}
