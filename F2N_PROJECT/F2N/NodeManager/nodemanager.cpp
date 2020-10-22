#include "nodemanager.h"
#include "ui_nodemanager.h"

#include "nodetablewgt.h"
NodeManager::NodeManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NodeManager)
{
    ui->setupUi(this);

    nodeTableWgt_1 = new NodeTableWgt;
    ui->tabWidget->addTab(nodeTableWgt_1,tr("通道-1"));
    nodeTableWgt_2 = new NodeTableWgt;
    ui->tabWidget->addTab(nodeTableWgt_2,tr("通道-2"));
    initConnect();
}

NodeManager::~NodeManager()
{
    delete ui;
}

void NodeManager::confShow()
{
    ui->tBtnEffect->setEnabled(true);
    nodeTableWgt_1->confShow(QString::number(1));
    nodeTableWgt_2->confShow(QString::number(2));
}

void NodeManager::initConnect()
{
    connect(ui->tBtnBack,&QToolButton::clicked,this,&NodeManager::sigNodeInfo);
    connect(ui->tBtnEffect,&QToolButton::clicked,this,&NodeManager::slotBtnEffect);

}

void NodeManager::slotBtnEffect()
{
    ui->tBtnEffect->setEnabled(false);
    emit sigSaveOk();
}



