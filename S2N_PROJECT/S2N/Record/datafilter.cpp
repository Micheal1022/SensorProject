#include "datafilter.h"
#include "ui_datafilter.h"
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QDebug>
#include <QButtonGroup>
DataFilter::DataFilter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataFilter)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::Dialog);
    setModal(true);
    initKey();
    initWidget();
    initConnect();

}

DataFilter::~DataFilter()
{
    delete ui;
    delete m_btnKeyGroup;
}

void DataFilter::initKey()
{
    m_btnKeyGroup = new QButtonGroup;
    m_btnKeyGroup->addButton(ui->tBtn_0,0);
    m_btnKeyGroup->addButton(ui->tBtn_1,1);
    m_btnKeyGroup->addButton(ui->tBtn_2,2);
    m_btnKeyGroup->addButton(ui->tBtn_3,3);
    m_btnKeyGroup->addButton(ui->tBtn_4,4);
    m_btnKeyGroup->addButton(ui->tBtn_5,5);
    m_btnKeyGroup->addButton(ui->tBtn_6,6);
    m_btnKeyGroup->addButton(ui->tBtn_7,7);
    m_btnKeyGroup->addButton(ui->tBtn_8,8);
    m_btnKeyGroup->addButton(ui->tBtn_9,9);
    connect(ui->tBtn_del,&QToolButton::clicked,this,&DataFilter::slotBtnDel);
    connect(m_btnKeyGroup,SIGNAL(buttonClicked(int)),this,SLOT(slotBtnClick(int)));
}

void DataFilter::initWidget()
{
    ui->lineEditPass->clear();
    ui->lineEditCanId->clear();
    ui->rBtnAllError->setChecked(true);
    ui->checkBoxPass->setChecked(false);
    ui->lineEditPass->setEnabled(false);
    ui->lineEditCanId->setEnabled(false);
    ui->checkBoxCanId->setChecked(false);
    ui->dTEditStop->setDateTime(QDateTime::currentDateTime());
}

void DataFilter::initConnect()
{
    connect(ui->tBtnOk,&QToolButton::clicked,this,&DataFilter::slotBtnOk);
    connect(ui->tBtnBack,&QToolButton::clicked,this,&DataFilter::slotBtnBack);
    connect(ui->rBtnHostError,&QRadioButton::clicked,this,&DataFilter::slotBtnHostError);
    connect(ui->tBtnStartUp,&QToolButton::clicked,ui->dTEditStart,&QDateTimeEdit::stepUp);
    connect(ui->tBtnStartDown,&QToolButton::clicked,ui->dTEditStart,&QDateTimeEdit::stepDown);
    connect(ui->tBtnStopUp,&QToolButton::clicked,ui->dTEditStop,&QDateTimeEdit::stepUp);
    connect(ui->tBtnStopDown,&QToolButton::clicked,ui->dTEditStop,&QDateTimeEdit::stepDown);
    connect(ui->checkBoxCanId,SIGNAL(clicked(bool)),this,SLOT(slotSetCanIdEnable(bool)));
    connect(ui->checkBoxPass,SIGNAL(clicked(bool)),this,SLOT(slotSetPassEnable(bool)));
}

void DataFilter::slotBtnOk()
{
    QString sqlQuery;
    sqlQuery = "select PASS,CANID,TYPE,STATE,VALUE,TIME,AREA from RECORD where";
    //Time
    QString startTime = QString::number(ui->dTEditStart->dateTime().toTime_t());
    QString stopTime  = QString::number(ui->dTEditStop->dateTime().toTime_t());
    sqlQuery += " TIME >= "+startTime+" and TIME <= "+stopTime;

    if (ui->rBtnNodeAlarm->isChecked()) {
        //节点报警
        sqlQuery += " and STATE = 1";
    } else if(ui->rBtnNodeError->isChecked()) {
        //节点故障
        sqlQuery += " and STATE = 2";
    } else if(ui->rBtnNodeOffLine->isChecked()) {
        //通讯故障
        sqlQuery += " and STATE = 3";
    } else if(ui->rBtnHostError->isChecked()) {
        //主机故障
        sqlQuery += " and STATE = 4 and STATE = 5";
    }

    if (!ui->rBtnHostError->isChecked()) {
        //PASS
        if (ui->checkBoxPass->isChecked()) {
            sqlQuery += " and PASS = "+ui->lineEditPass->text();
        }
        //CANID
        if (ui->checkBoxCanId->isChecked()) {
            sqlQuery += " and CANID = "+ui->lineEditCanId->text();
        }
    }
    sqlQuery += " order by TIME desc;";
    this->close();
    emit sigSqlQuery(sqlQuery);
}

void DataFilter::slotBtnBack()
{
    this->hide();
}

void DataFilter::slotBtnDel()
{
    if (this->focusWidget()->inherits("QLineEdit")) {
        QLineEdit *edit = qobject_cast<QLineEdit*>(focusWidget());
        edit->clear();
    }
}

void DataFilter::slotBtnHostError()
{
    ui->lineEditPass->clear();
    ui->lineEditCanId->clear();
    ui->lineEditPass->setEnabled(false);
    ui->lineEditCanId->setEnabled(false);
    ui->checkBoxPass->setChecked(false);
    ui->checkBoxCanId->setChecked(false);
}

void DataFilter::slotBtnClick(int index)
{
    QString text = m_btnKeyGroup->button(index)->text();;
    if (focusWidget()->inherits("QLineEdit")) {
        QLineEdit *edit = qobject_cast<QLineEdit*>(focusWidget());
        QString curStr = edit->text();
        edit->setText(curStr+text);
    }
}

void DataFilter::slotSetCanIdEnable(bool enable)
{
    ui->lineEditCanId->setEnabled(enable);
    if (enable != true) {
        ui->lineEditCanId->clear();
    }
}

void DataFilter::slotSetPassEnable(bool enable)
{
    ui->lineEditPass->setEnabled(enable);
    if (enable != true) {
        ui->lineEditPass->clear();
    }
}
