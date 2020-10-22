#include "nodetablewgt.h"
#include "ui_nodetablewdt.h"
#include <QLabel>
#include <QTableWidget>
#include <QButtonGroup>
#include <QMessageBox>
#include <SQLite/sqlite.h>
#include "MsgBox/msgbox.h"
NodeTableWgt::NodeTableWgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NodeTableWgt)
{
    ui->setupUi(this);
    initKey();
    initConnect();
    initTableWidget(ui->tableWgt);
    QRegExp regExpDevType("^[1-9][0-9][0-9][0-9]$");
    ui->lineEditRatio->setValidator(new QRegExpValidator(regExpDevType, this));
    ui->lineEditNumber->setValidator(new QRegExpValidator(regExpDevType, this));

}

void NodeTableWgt::confShow(QString pass)
{
    m_pass = pass;
    m_nodeInfoList.clear();
    QSqlDatabase db = SQLite::openConnection();
    m_nodeInfoList = SQLite::getNodeInfoList(db,pass);
    SQLite::closeConnection(db);

    m_nodeCount = m_nodeInfoList.count();
    ui->lbNodeCount->setText(QString::number(m_nodeCount));

    confTableItem(ui->tableWgt,m_nodeInfoList);
}

void NodeTableWgt::initConnect()
{
    connect(ui->tBtnSave,&QToolButton::clicked,this,&NodeTableWgt::slotBtnSaveConf);
    connect(ui->tableWgt,&QTableWidget::cellDoubleClicked,this,&NodeTableWgt::slotCellDoubleClicked);
}

void NodeTableWgt::confTableItem(QTableWidget *tableWidget, QList<QStringList> nodeInfoList)
{
    tableWidget->setRowCount(nodeInfoList.count());
    QFont ft("Courier 10 Pitch",11);
    QTableWidgetItem *item;
    for(int row = 0; row < nodeInfoList.count();row++)
    {
        QStringList nodeInfo = nodeInfoList.value(row);
        tableWidget->setRowHeight(row,27);
        for(int column = 0;column < tableWidget->columnCount();column++)
        {
            item = new QTableWidgetItem;
            item->setFont(ft);
            item->setTextAlignment(Qt::AlignCenter);
            item->setTextColor(QColor(255,255,255));

            switch (column)
            {
            case 0://地址
                item->setText(nodeInfo.value(column));
                break;
            case 1://启用
                if(nodeInfo.value(column) == QString::number(0))
                {
                    item->setText(tr("禁用"));
                    item->setTextColor(QColor(255,0,0));
                }
                else
                {
                    item->setText(tr("启用"));
                    item->setTextColor(QColor(0,255,0));
                }
                break;
            case 2://变比值
                item->setText(nodeInfo.value(column));
                break;
            case 3://详细地址
                item->setText(nodeInfo.value(column));
                break;
            }
            tableWidget->setItem(row,column,item);
        }
    }
}

void NodeTableWgt::initTableWidget(QTableWidget *tableWidget)
{
    QLabel *label = new QLabel(tableWidget);
    label->show();
    label->setFixedSize(34,30);
    label->setAlignment(Qt::AlignVCenter);
    label->setStyleSheet("color: blue;font: 12pt 'Courier 10 Pitch'; background-color: rgb(255, 255, 255);");
    label->setText("序号");
    label->move(tableWidget->pos().x()+2,tableWidget->pos().y()+2);

    QStringList headList;
    headList<<tr("地址")<<tr("状态")<<tr("变比值")<<tr("详细地址");

    tableWidget->setColumnCount(headList.size());
    tableWidget->setHorizontalHeaderLabels(headList);
    tableWidget->horizontalHeader()->setFixedHeight(31);
    tableWidget->horizontalHeader()->setDefaultSectionSize(140);
    QString hstyleStr = "QHeaderView::section{font: 13pt 'Courier 10 Pitch'; background-color: rgb(28, 183, 200);"
                        "color: black;}";
    QString vstyleStr = "QHeaderView::section{font: 11pt 'Courier 10 Pitch'; background-color: rgb(255, 255, 255);"
                        "color: blue;border:1px solid rgb(0, 0, 0);}";
    //    tableWidget->setFocusPolicy(Qt::NoFocus); //去除选中虚线框
    tableWidget->horizontalHeader()->setStyleSheet(hstyleStr);
    tableWidget->verticalHeader()->setStyleSheet(vstyleStr);
    tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);
    tableWidget->verticalHeader()->setFixedWidth(35);
    tableWidget->verticalHeader()->setEnabled(false);
    tableWidget->horizontalHeader()->setEnabled(false);
    tableWidget->horizontalHeader()->setVisible(true);//设置表头显示
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setHighlightSections(false);

    tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);//单元格不可编
    tableWidget->setSelectionBehavior (QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    tableWidget->setSelectionMode (QAbstractItemView::SingleSelection); //设置选择模式，选择单行

    tableWidget->setColumnWidth(0,70);
    tableWidget->setColumnWidth(1,70);
    tableWidget->setColumnWidth(2,70);
    tableWidget->setColumnWidth(3,230);
}

void NodeTableWgt::slotBtnOk()
{
    if(ui->lineEditNumber->text().isEmpty() != true && ui->lineEditRatio->text().isEmpty() != true)
    {
        int number = ui->lineEditNumber->text().toInt();
        if(number > m_nodeCount)
        {
            MsgBox::showInformation(this,tr("操作提示"),tr("超过最大序号!"),tr("关闭"));
            ui->lineEditNumber->clear();
        }
        else
        {
            ui->tableWgt->item(number-1,2)->setText(ui->lineEditRatio->text());
        }
    }
    else
    {
        MsgBox::showInformation(this,tr("操作提示"),tr("序号和变比值不能为空!"),tr("关闭"));
    }
}

void NodeTableWgt::slotBtnSaveConf()
{
    bool flag = true;
    for(int row = 0; row < m_nodeInfoList.count();row++)
    {
        QStringList nodeInfo = m_nodeInfoList.value(row);
        QString canIdStr = nodeInfo.value(0);
        QString usedStrPre  = nodeInfo.value(1);
        QString ratioStrPre = nodeInfo.value(2);

        QString usedStrCur  = nodeInfo.value(1);
        if(ui->tableWgt->item(row,1)->text() == tr("启用"))
        {
            usedStrCur = QString::number(1);
        }
        else
        {
            usedStrCur = QString::number(0);
        }
        QString ratioStrCur = ui->tableWgt->item(row,2)->text();

        if(usedStrPre != usedStrCur && ratioStrPre != ratioStrCur)
        {
            QSqlDatabase db = SQLite::openConnection();
            flag = SQLite::updateNodeInfo(db,m_pass,canIdStr,usedStrCur,ratioStrCur);
            SQLite::closeConnection(db);
            if(flag == false)
            {
                MsgBox::showInformation(this,tr("操作提示!"),tr("保存失败"),tr("关闭"));
                return;
            }
        }
        else
        {
            if(usedStrPre != usedStrCur)
            {
                QSqlDatabase db = SQLite::openConnection();
                flag = SQLite::updateNodeInfo(db,m_pass,canIdStr,usedStrCur,ratioStrPre);
                SQLite::closeConnection(db);
                if(flag == false)
                {
                    MsgBox::showInformation(this,tr("操作提示!"),tr("保存失败"),tr("关闭"));
                    return;
                }
            }

            if(ratioStrPre != ratioStrCur)
            {
                QSqlDatabase db = SQLite::openConnection();
                flag = SQLite::updateNodeInfo(db,m_pass,canIdStr,usedStrPre,ratioStrCur);
                SQLite::closeConnection(db);
                if(flag == false)
                {
                    MsgBox::showInformation(this,tr("操作提示!"),tr("保存失败"),tr("关闭"));
                    return;
                }
            }
        }
    }
    if(flag == true)
    {
        MsgBox::showInformation(this,tr("操作提示!"),tr("保存成功"),tr("关闭"));
    }
}

void NodeTableWgt::slotCellDoubleClicked(int row, int column)
{
    Q_UNUSED(column)
    if(ui->tableWgt->item(row,1)->text() == tr("启用"))
    {
        ui->tableWgt->item(row,1)->setText(tr("禁用"));
        ui->tableWgt->item(row,1)->setTextColor(QColor(255,0,0));
    }
    else
    {
        ui->tableWgt->item(row,1)->setText(tr("启用"));
        ui->tableWgt->item(row,1)->setTextColor(QColor(0,255,0));
    }
}

NodeTableWgt::~NodeTableWgt()
{
    delete ui;
}

void NodeTableWgt::initKey()
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
    connect(ui->tBtnOk,&QToolButton::clicked,this,&NodeTableWgt::slotBtnOk);
    connect(m_btnKeyGroup,SIGNAL(buttonClicked(int)),this,SLOT(slotBtnClick(int)));
}

void NodeTableWgt::slotBtnClick(int index)
{
    QString text = m_btnKeyGroup->button(index)->text();;
    if(focusWidget()->inherits("QLineEdit"))
    {
        QLineEdit *edit = qobject_cast<QLineEdit*>(focusWidget());
        QString curStr = edit->text();
        edit->setText(curStr+text);
    }
}

void NodeTableWgt::slotBtnDel()
{
    if(this->focusWidget()->inherits("QLineEdit"))
    {
        QLineEdit *edit = qobject_cast<QLineEdit*>(focusWidget());
        edit->clear();
    }
}
