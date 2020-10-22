#include "nodeinfo.h"
#include "ui_nodeinfo.h"
#include "MsgBox/msgbox.h"
NodeInfo::NodeInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NodeInfo)
{
    ui->setupUi(this);
    initKey();
}

NodeInfo::~NodeInfo()
{
    delete ui;
}

void NodeInfo::confShow()
{
    ui->lineEditID->clear();
    ui->lineEditPass->clear();
    ui->lineEditBaseLeak->clear();
    ui->lineEditAlarmLeak->clear();
    ui->lineEditAlarmTemp->clear();
}

void NodeInfo::initKey()
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
    m_btnKeyGroup->addButton(ui->tBtn_del,10);
    m_btnKeyGroup->addButton(ui->tBtnOK,11);
    m_btnKeyGroup->addButton(ui->tBtnSetLeak,12);
    m_btnKeyGroup->addButton(ui->tBtnSetTemp,13);
    m_btnKeyGroup->addButton(ui->tBtnBackNodeView,14);
    connect(m_btnKeyGroup,SIGNAL(buttonClicked(int)),this,SLOT(slotBtnClick(int)));
}

void NodeInfo::setAlarmData()
{
    if (ui->lineEditPass->text().isEmpty() || ui->lineEditID->text().isEmpty()) {
        MsgBox::showInformation(this,tr("提示"),tr("通道和地址不能为空!!!"),tr("关闭"));
        return;
    }
    if (0 == ui->stackedWidget->currentIndex()) {
        if (ui->lineEditBaseLeak->text().isEmpty() || ui->lineEditAlarmLeak->text().isEmpty()) {
            MsgBox::showInformation(this,tr("提示"),tr("固有漏电和漏电阈值不能为空!!!"),tr("关闭"));
            return;
        } else if (ui->lineEditAlarmLeak->text().toInt() < 300 || ui->lineEditAlarmLeak->text().toInt() > 2000) {
            MsgBox::showInformation(this,tr("提示"),tr("漏电阈值不符合要求!!!"),tr("关闭"));
            return;
        } else if (ui->lineEditBaseLeak->text().toInt() > 2000) {
            MsgBox::showInformation(this,tr("提示"),tr("漏电固有不符合要求!!!"),tr("关闭"));
            return;
        } else {
            int pass = ui->lineEditPass->text().toInt();
            qint32 ID   = ui->lineEditID->text().toInt();
            quint16 alarmLeak = ui->lineEditAlarmLeak->text().toInt();
            quint16 baseLeak = ui->lineEditBaseLeak->text().toInt();
            emit sigSendAlarmData(pass,ID,2,alarmLeak,baseLeak);

        }
    } else {
        if (ui->lineEditAlarmTemp->text().isEmpty()) {
            MsgBox::showInformation(this,tr("提示"),tr("温度阈值不能为空!!!"),tr("关闭"));
            return;
        } else if (ui->lineEditAlarmTemp->text().toInt() < 55 || ui->lineEditAlarmTemp->text().toInt() > 110) {
            MsgBox::showInformation(this,tr("提示"),tr("温度阈值不符合要求!!!"),tr("关闭"));
            return;
        } else {
            int pass = ui->lineEditPass->text().toInt();
            quint16 ID   = ui->lineEditID->text().toInt();
            quint16 alarmTemp = ui->lineEditAlarmTemp->text().toInt();
            emit sigSendAlarmData(pass,ID,3,alarmTemp,0);
        }
    }
    MsgBox::showInformation(this,tr("提示"),tr("设置阈值已经发送!!!"),tr("关闭"));

}

void NodeInfo::slotBtnClick(int index)
{
    switch (index) {
    case 10:
        if (focusWidget()->inherits("QLineEdit")) {
            QLineEdit *edit = qobject_cast<QLineEdit*>(focusWidget());
            edit->clear();
        }
        break;
    case 11:
        setAlarmData();
        break;
    case 12:
        ui->stackedWidget->setCurrentIndex(0);
        break;
    case 13:
        ui->stackedWidget->setCurrentIndex(1);
        break;
    case 14:
        emit sigNodeView();
        break;
    default:
        if (focusWidget()->inherits("QLineEdit")) {
            QLineEdit *edit = qobject_cast<QLineEdit*>(focusWidget());
            QString curStr = edit->text();
            edit->setText(curStr + m_btnKeyGroup->button(index)->text());
        }
        break;
    }
}
