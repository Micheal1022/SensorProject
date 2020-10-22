#include "login.h"
#include "ui_login.h"

#include "MsgBox/msgbox.h"
Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    initKey();
}

Login::~Login()
{
    delete ui;
}

void Login::confShow()
{
    ui->lineEditPwd->clear();
    ui->lineEditPwd->setFocus();
    ui->rBtnUser->setChecked(true);
}

void Login::initKey()
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
    connect(m_btnKeyGroup,SIGNAL(buttonClicked(int)),this,SLOT(slotBtnClick(int)));
    connect(ui->tBtnOk,  &QToolButton::clicked,this,&Login::slotBtnOk);
    connect(ui->tBtnClear,&QToolButton::clicked,this,&Login::slotBtnClear);
    connect(ui->tBtnBack,&QToolButton::clicked,this,&Login::slotBtnBackNodeView);
}

void Login::slotBtnOk()
{
    QString pwdStr = ui->lineEditPwd->text();
    if (ui->rBtnAdmin->isChecked() == true) {
        if (pwdStr.isEmpty() == true || pwdStr != QString::number(222222)) {
            MsgBox::showInformation(this,tr("操作提示"),tr("密码错误!"),tr("关闭"));
        } else {
            emit sigLoginOk(SUPER);
        }
    } else if (ui->rBtnUser->isChecked() == true) {
        if (pwdStr.isEmpty() == true || pwdStr != QString::number(111111)) {
            MsgBox::showInformation(this,tr("操作提示"),tr("密码错误!"),tr("关闭"));
        } else {
            emit sigLoginOk(ADMIN);
        }
    }
}

void Login::slotBtnBackNodeView()
{
    emit sigNodeView();//返回到监视界面
}

void Login::slotBtnClear()
{
    ui->lineEditPwd->clear();
}

void Login::slotBtnClick(int index)
{
    QString text = m_btnKeyGroup->button(index)->text();;
    if (focusWidget()->inherits("QLineEdit")) {
        QLineEdit *edit = qobject_cast<QLineEdit*>(focusWidget());
        QString curStr = edit->text();
        edit->setText(curStr+text);
    }
}
