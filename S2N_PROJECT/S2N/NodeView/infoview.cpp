#include "infoview.h"
#include "ui_infoview.h"
#include <QDebug>
#define MOD_LEAK    2
#define MOD_TEMP    3
#define MOD_EARC    4

#define MOD_ALARM 1
#define MOD_ERROR 2
#define MOD_DROP  3

#define PASS    0
#define CANID   1
#define TYPE    2
#define STATE   3
#define VALUE   4
#define TIME    5
#define ADDR    6

InfoView::InfoView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoView)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Dialog);
    setModal(true);
    m_curIndex = 0;
    m_dataList.clear();
    initConnect();
}

InfoView::~InfoView()
{
    delete ui;
}

void InfoView::initConf(QList<QStringList> dataList, InfoView::DataType type)
{
    (type == AlarmData) ? ui->lbTitle->setText(tr("报警信息")) : ui->lbTitle->setText(tr("故障信息"));

    int count = dataList.count();
    ui->lbNum->setText(QString::number(count));
    if (count > 0) {
        m_dataList = dataList;
        ui->lbIndex->setText("1");
        QStringList stringList = m_dataList.first();
        infoShow(stringList);
    } else {
        ui->lbIndex->setText("0");
        ui->tBtnLastItem->setEnabled(false);
        ui->tBtnNestItem->setEnabled(false);
        ui->tBtnTailItem->setEnabled(false);
        ui->tBtnHeadItem->setEnabled(false);
    }
}

void InfoView::initConnect()
{
    connect(ui->tBtnBack,SIGNAL(clicked(bool)),this,SLOT(slotBtnBack()));
    connect(ui->tBtnHeadItem,SIGNAL(clicked(bool)),this,SLOT(slotBtnHeadItem()));
    connect(ui->tBtnTailItem,SIGNAL(clicked(bool)),this,SLOT(slotBtnTailItem()));
    connect(ui->tBtnLastItem,SIGNAL(clicked(bool)),this,SLOT(slotBtnLastItem()));
    connect(ui->tBtnNestItem,SIGNAL(clicked(bool)),this,SLOT(slotBtnNestItem()));
}

void InfoView::infoShow(QStringList stringList)
{
    QString nodeType = stringList.value(TYPE);
    if (nodeType == tr("漏电")) {
        ui->lbValue->setVisible(true);
        ui->lbValue_1->setVisible(true);
    } else if (nodeType == tr("温度")) {
        ui->lbValue->setVisible(true);
        ui->lbValue_1->setVisible(true);
    } else {
        ui->lbValue->setVisible(false);
        ui->lbValue_1->setVisible(false);
    }
    ui->lbType->setText(nodeType);
    QString nodeState = stringList.value(STATE);
    if (nodeState != tr("节点报警")) {
        ui->lbValue->setVisible(false);
        ui->lbValue_1->setVisible(false);
    }
    ui->lbState->setText(nodeState);

    ui->lbTime->setText(stringList.value(TIME));
    ui->lbArea->setText(stringList.value(ADDR));
    ui->lbValue->setText(stringList.value(VALUE));
    ui->lbNodeAddr->setText(stringList.value(PASS)+tr("-")+stringList.value(CANID));
}

void InfoView::slotBtnHeadItem()
{
    m_curIndex = 0;
    ui->lbIndex->setText(QString::number(m_curIndex+1));
    QStringList stringList = m_dataList.first();
    infoShow(stringList);
}

void InfoView::slotBtnTailItem()
{
    m_curIndex = m_dataList.count() - 1;
    ui->lbIndex->setText(QString::number(m_curIndex+1));
    QStringList stringList = m_dataList.last();
    infoShow(stringList);
}

void InfoView::slotBtnNestItem()
{
    m_curIndex++;
    if(m_curIndex == m_dataList.count())
        m_curIndex = 0;

    ui->lbIndex->setText(QString::number(m_curIndex+1));
    QStringList stringList = m_dataList.value(m_curIndex);
    infoShow(stringList);
}

void InfoView::slotBtnLastItem()
{
    m_curIndex--;
    if(m_curIndex < 0)
        m_curIndex = m_dataList.count() - 1;

    ui->lbIndex->setText(QString::number(m_curIndex+1));
    QStringList stringList = m_dataList.value(m_curIndex);
    infoShow(stringList);
}

void InfoView::slotBtnBack()
{
    this->close();
}
