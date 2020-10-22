#include "stateinfo.h"
#include "ui_stateinfo.h"

StateInfo::StateInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StateInfo)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Dialog);
    setModal(true);
    m_curIndex = 0;
    m_dataList.clear();
    initConnect();
}

StateInfo::~StateInfo()
{
    delete ui;
}

void StateInfo::initConf(QList<QStringList> dataList, StateInfo::DataType type)
{
    (type == PowerData) ? ui->lbTitle->setText(tr("供电中断")) : ui->lbTitle->setText(tr("故障信息"));

    int count = dataList.count();
    ui->lbNum->setText(QString::number(count));
    if(count > 0)
    {
        m_dataList = dataList;
        ui->lbIndex->setText("1");
        QStringList stringList = m_dataList.first();
        infoShow(stringList);
    }
    else
    {
        ui->lbIndex->setText("0");
        ui->tBtnLastItem->setEnabled(false);
        ui->tBtnNestItem->setEnabled(false);
        ui->tBtnTailItem->setEnabled(false);
        ui->tBtnHeadItem->setEnabled(false);
    }
}

void StateInfo::initConnect()
{
    connect(ui->tBtnBack,SIGNAL(clicked(bool)),this,SLOT(slotBtnBack()));
    connect(ui->tBtnHeadItem,SIGNAL(clicked(bool)),this,SLOT(slotBtnHeadItem()));
    connect(ui->tBtnTailItem,SIGNAL(clicked(bool)),this,SLOT(slotBtnTailItem()));
    connect(ui->tBtnLastItem,SIGNAL(clicked(bool)),this,SLOT(slotBtnLastItem()));
    connect(ui->tBtnNestItem,SIGNAL(clicked(bool)),this,SLOT(slotBtnNestItem()));


}

void StateInfo::infoShow(QStringList stringList)
{
    ui->lbNodeAddr->setText(stringList.value(PASS)+tr("-")+stringList.value(CANID));
    ui->lbState->setText(stringList.value(STS));
    ui->lbTime->setText(stringList.value(TIME));
    ui->lbArea->setText(stringList.value(ADDR));
}

void StateInfo::slotBtnHeadItem()
{
    m_curIndex = 0;
    ui->lbIndex->setText(QString::number(m_curIndex+1));
    QStringList stringList = m_dataList.first();
    infoShow(stringList);
}

void StateInfo::slotBtnTailItem()
{
    m_curIndex = m_dataList.count() - 1;
    ui->lbIndex->setText(QString::number(m_curIndex+1));
    QStringList stringList = m_dataList.last();
    infoShow(stringList);
}

void StateInfo::slotBtnNestItem()
{
    m_curIndex++;
    if(m_curIndex == m_dataList.count())
        m_curIndex = 0;

    ui->lbIndex->setText(QString::number(m_curIndex+1));
    QStringList stringList = m_dataList.value(m_curIndex);
    infoShow(stringList);
}

void StateInfo::slotBtnLastItem()
{
    m_curIndex--;
    if(m_curIndex < 0)
        m_curIndex = m_dataList.count() - 1;

    ui->lbIndex->setText(QString::number(m_curIndex+1));
    QStringList stringList = m_dataList.value(m_curIndex);
    infoShow(stringList);
}

void StateInfo::slotBtnBack()
{
    this->close();
}
