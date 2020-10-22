#include "userlogin.h"
#include "ui_userlogin.h"
#include <QProcess>
#include <MsgBox/msgbox.h>
UserLogin::UserLogin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserLogin)
{
    ui->setupUi(this);
    initKey();

}

UserLogin::~UserLogin()
{
    delete ui;
}

void UserLogin::confShow()
{
    ui->lineEditPwd->clear();
    ui->lineEditPwd->setFocus();
    ui->rBtnUser->setChecked(true);
}

void UserLogin::initKey()
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
    connect(ui->tBtn_del,&QToolButton::clicked,this,&UserLogin::slotBtnDel);
    connect(m_btnKeyGroup,SIGNAL(buttonClicked(int)),this,SLOT(slotBtnClick(int)));
    connect(ui->tBtnOk,&QToolButton::clicked,this,&UserLogin::slotBtnOk);
    connect(ui->tBtnBack,&QToolButton::clicked,this,&UserLogin::slotBtnBack);
}

void UserLogin::slotBtnOk()
{
    QString pwdStr = ui->lineEditPwd->text();
    if(ui->rBtnAdmin->isChecked() == true)
    {
        if(pwdStr.isEmpty() == true || pwdStr != QString::number(222222))
        {
            MsgBox::showInformation(this,tr("操作提示"),tr("密码错误!"),tr("关闭"));
            return;
        }
    }
    else if(ui->rBtnUser->isChecked() == true)
    {
        if(pwdStr.isEmpty() == true || pwdStr != QString::number(111111))
        {
            MsgBox::showInformation(this,tr("操作提示"),tr("密码错误!"),tr("关闭"));
            return;
        }
    }
    emit sigLogin();
    emit sigNodeInfo();
}

void UserLogin::slotBtnBack()
{
    emit sigNodeInfo();
}

void UserLogin::slotBtnDel()
{
    if(this->focusWidget()->inherits("QLineEdit"))
    {
        QLineEdit *edit = qobject_cast<QLineEdit*>(focusWidget());
        edit->clear();
    }
}

void UserLogin::slotBtnClick(int index)
{
    QString text = m_btnKeyGroup->button(index)->text();;
    if(focusWidget()->inherits("QLineEdit"))
    {
        QLineEdit *edit = qobject_cast<QLineEdit*>(focusWidget());
        QString curStr = edit->text();
        edit->setText(curStr+text);
    }
}


