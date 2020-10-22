#include "recordquery.h"
#include "ui_recordquery.h"
#include "datafilter.h"
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QTextEdit>
#include <SQLite/sqlite.h>
#include "MsgBox/msgbox.h"
#include "MyPrint/myprint.h"
RecordQuery::RecordQuery(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecordQuery)
{
    ui->setupUi(this);
    initWidget();
    initConnect();
    initTableWidget(ui->tableWgtRecord);
}

RecordQuery::~RecordQuery()
{
    delete ui;
}

void RecordQuery::confShow(GlobalData::UserType user)
{
    switch (user) {
    case GlobalData::User:
        ui->tBtnExport->setEnabled(true);
        break;
    case GlobalData::Admin:
        ui->tBtnDel->setEnabled(true);
        ui->tBtnExport->setEnabled(true);
        break;
    }
    QString dateTimeStr = QString::number(QDateTime::currentDateTime().toTime_t());
    m_queryStr = "select PASS,CANID,STS,TIME,AREA from RECORD where TIME <= "+dateTimeStr+" order by TIME desc;";
}

void RecordQuery::initConnect()
{
    connect(ui->tBtnBack,  &QToolButton::clicked,this,&RecordQuery::slotBtnBack);
    connect(ui->tBtnDel,   &QToolButton::clicked,this,&RecordQuery::slotBtnDelete);
    connect(ui->tBtnPrint, &QToolButton::clicked,this,&RecordQuery::slotBtnPrint);
    connect(ui->tBtnQuery, &QToolButton::clicked,this,&RecordQuery::slotBtnQuery);
    connect(ui->tBtnExport,&QToolButton::clicked,this,&RecordQuery::slotBtnExport);
    connect(ui->tBtnFilter,&QToolButton::clicked,this,&RecordQuery::slotBtnFilter);
}

void RecordQuery::initTableWidget(QTableWidget *tableWidget)
{

    QStringList headList;
    headList<<tr("通道")<<tr("地址")<<tr("状态")<<tr("时间")<<tr("区域");
    tableWidget->setColumnCount(headList.size());
    tableWidget->setHorizontalHeaderLabels(headList);
    tableWidget->horizontalHeader()->setFixedHeight(30);
    tableWidget->horizontalHeader()->setDefaultSectionSize(140);
    QString styleStr = "QHeaderView::section{font: 13pt '文泉驿等宽微米黑'; background-color: rgb(28, 183, 200);"
                       "color: black;}";
    QString vstyleStr ="QHeaderView::section{font: 12pt '文泉驿等宽微米黑';background-color: rgb(15, 41, 68);"
                       "color: white;}";
    //tableWidget->setFocusPolicy(Qt::NoFocus); //去除选中虚线框
    tableWidget->horizontalHeader()->setStyleSheet(styleStr);
    tableWidget->verticalHeader()->setStyleSheet(vstyleStr);
    tableWidget->verticalHeader()->setFixedWidth(30);
    tableWidget->verticalHeader()->setEnabled(false);
    tableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);
    tableWidget->horizontalHeader()->setEnabled(false);
    tableWidget->horizontalHeader()->setVisible(true);//设置表头显示
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setHighlightSections(false);

    tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);//单元格不可编
    tableWidget->setSelectionBehavior (QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    //tableWidget->setSelectionMode (QAbstractItemView::SingleSelection); //设置选择模式，选择单行

    tableWidget->setColumnWidth(0,80);
    tableWidget->setColumnWidth(1,100);
    tableWidget->setColumnWidth(2,100);
    tableWidget->setColumnWidth(3,250);
    tableWidget->setColumnWidth(4,300);

}

void RecordQuery::initWidget()
{
    setWindowFlags(Qt::FramelessWindowHint);
    m_dataFilter = new DataFilter;
    MyPrint::initSerialPort(QString("/dev/ttyS0"));
    m_formatTime = QString("yyyy-MM-dd hh:mm:ss");
    ui->tBtnDel->setEnabled(false);
    ui->tBtnExport->setEnabled(false);
    ui->lbCountNum->setText("0");
    ui->tBtnExport->setVisible(false);
    QString dateTimeStr = QString::number(QDateTime::currentDateTime().toTime_t());
    m_queryStr = "select PASS,CANID,STS,TIME,AREA from RECORD where TIME <= "+dateTimeStr+" order by TIME desc;";
}

void RecordQuery::showQuery(QString queryStr, QTableWidget *tableWidget)
{
    tableWidget->clearContents();
    QSqlDatabase db = SQLite::openConnection();
    QString sqlCount = "select count(*) "+queryStr.right(queryStr.size() - queryStr.indexOf("from"));
    int rowCount = SQLite::getRowCount(db,sqlCount);
    if(rowCount <= 0)
    {
        tableWidget->setRowCount(0);
        SQLite::closeConnection(db);
        MsgBox::showInformation(this,tr("查询提示"),tr("没有符合条件的记录!"),tr("关闭"));
        return;
    }

    tableWidget->setRowCount(rowCount);
    ui->lbCountNum->setText(QString::number(rowCount));
    QFont ft("文泉驿等宽微米黑",13);
    QTableWidgetItem *item;
    QSqlQuery query(db);
    if(query.exec(queryStr))
    {
        int row = 0;
        tableWidget->setRowHeight(row,25);
        while (query.next())
        {
            uint dt;
            QDateTime dateTime;
            for(int column = 0;column < tableWidget->columnCount(); column++)
            {
                item = new QTableWidgetItem;
                item->setFont(ft);
                item->setTextColor(QColor(255,255,255));
                item->setTextAlignment(Qt::AlignCenter);

                switch (column) {
                case Pass:
                    item->setText(query.value(column).toString());
                    break;
                case CanId:
                    item->setText(query.value(column).toString());
                    break;
                case State:
                    item->setTextColor(QColor(255,255,0));
                    switch (query.value(column).toInt()) {
                    case NodeOverCurrent://过流
                        //item->setTextColor(QColor(255,0,0));
                        item->setText(tr("节点过流"));
                        break;
                    case NodeLackPhase://错相
                        item->setText(tr("节点错相"));
                        break;
                    case NodeOverVoltage://过压
                        item->setText(tr("节点过压"));
                        break;
                    case NodeUnderVoltage://欠压
                        item->setText(tr("节点欠压"));
                        break;
                    case NodePowerLost://供电中断
                        item->setTextColor(QColor(255,0,0));
                        item->setText(tr("供电中断"));
                        break;
                    case NodeCanBusError://通讯短路
                        item->setText(tr("通讯故障"));
                        break;
                    case MainPowerError://主电故障
                        item->setText(tr("主电故障"));
                        break;
                    case BackPowerError://备电故障
                        item->setText(tr("备电故障"));
                        break;
                    }
                    break;
                case Time:
                    dt = query.value(column).toUInt();
                    dateTime.setTime_t(dt);
                    item->setText(dateTime.toString(m_formatTime));
                    break;
                case Area:
                    item->setText(query.value(column).toString());
                    break;
                }
                tableWidget->setItem(row,column,item);
            }
            row++;
        }
    }
    SQLite::closeConnection(db);
}

void RecordQuery::slotBtnBack()
{
    emit sigNodeInfo();
}

void RecordQuery::slotBtnQuery()
{
    showQuery(m_queryStr,ui->tableWgtRecord);
}

void RecordQuery::slotQueryStr(QString queryStr)
{
    m_queryStr = queryStr;
    showQuery(m_queryStr,ui->tableWgtRecord);
}

void RecordQuery::slotBtnPrint()
{
    if(ui->tableWgtRecord->currentRow() < 0)
    {
        MsgBox::showInformation(this,tr("打印提示"),tr("未选中要打印的内容!"),tr("关闭"));
        return;
    }

    for(int row = 0;row < ui->tableWgtRecord->rowCount();row++)
    {
        if(ui->tableWgtRecord->item(row,1)->isSelected())
        {
            QString pass  = ui->tableWgtRecord->item(row,0)->text();
            QString canId = ui->tableWgtRecord->item(row,1)->text();
            QString state = ui->tableWgtRecord->item(row,2)->text();
            QString time  = ui->tableWgtRecord->item(row,3)->text();
            QString area  = ui->tableWgtRecord->item(row,4)->text();
            qDebug()<<pass<<canId<<state<<time<<area;
            MyPrint::dataPring(pass,canId,state,time,area);
        }
    }
}

void RecordQuery::slotBtnDelete()
{
    int ret = MsgBox::showQuestion(this,tr("删除提示"),tr("您确定要删除数据?"),tr("部分"),tr("所有"));
    if (ret == 0) {
        for(int row = 0;row < ui->tableWgtRecord->rowCount();row++)
        {
            if(ui->tableWgtRecord->item(row,1)->isSelected())
            {
                int pass  = ui->tableWgtRecord->item(row,Pass)->text().toUInt();
                int canId = ui->tableWgtRecord->item(row,CanId)->text().toUInt();
                QString nodeState = ui->tableWgtRecord->item(row,State)->text();
                int state = 0;
                if(nodeState == tr("节点过流"))
                {
                    state = NodeOverCurrent;
                }
                else if(nodeState == tr("节点错相"))
                {
                    state = NodeLackPhase;
                }
                else if(nodeState == tr("节点过压"))
                {
                    state = NodeOverVoltage;
                }
                else if(nodeState == tr("节点欠压"))
                {
                    state = NodeUnderVoltage;
                }
                else if(nodeState == tr("供电中断"))
                {
                    state = NodePowerLost;
                }
                else if(nodeState == tr("通讯故障"))
                {
                    state = NodeCanBusError;
                }
                else if(nodeState == tr("主电故障"))
                {
                    state = MainPowerError;
                }
                else if(nodeState == tr("备电故障"))
                {
                    state = BackPowerError;
                }

                uint alarmTime = QDateTime::fromString(ui->tableWgtRecord->item(row,Time)->text(),m_formatTime).toTime_t();
                QSqlDatabase db = SQLite::openConnection();
                SQLite::delelteRecordItem(db,pass,canId,state,alarmTime);
                SQLite::closeConnection(db);
            }
        }
    } else if (ret == 1) {
        QSqlDatabase db = SQLite::openConnection();
        SQLite::delelteAllItem(db);
        SQLite::closeConnection(db);
    }
    showQuery(m_queryStr,ui->tableWgtRecord);
}

void RecordQuery::slotBtnExport()
{
    QString fileName = QString(tr("History_")+QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    QString filePath = QFileDialog::getSaveFileName(this,tr("历史记录导出..."),fileName,tr("EXCEL files(*.xls);;HTML-Files(*.txt);;"));
    if(filePath.isEmpty())
    {
        return;
    }
    int rowCount = ui->tableWgtRecord->rowCount();
    int columnCount = ui->tableWgtRecord->columnCount();
    QList<QString> list;
    //添加列标题
    QString HeaderRow;
    for(int column = 0;column < columnCount;column++)
    {
        HeaderRow.append(ui->tableWgtRecord->horizontalHeaderItem(column)->text()+"\t");
    }
    list.append(HeaderRow);
    //添加item内容
    for(int row = 0;row < rowCount;row++)
    {
        QString rowStr = "";
        for(int j = 0;j < columnCount;j++)
        {
            rowStr += ui->tableWgtRecord->item(row,j)->text() + "\t";
        }
        list.append(rowStr);
    }
    QTextEdit textEdit;
    for(int i=0;i<list.size();i++)
    {
        textEdit.append(list.at(i));
    }
    //文件保存
    QFile file(filePath);
    if(file.open(QFile::WriteOnly | QIODevice::Text))
    {
        QTextStream ts(&file);
        ts<<textEdit.document()->toPlainText();
        file.close();
    }
    MsgBox::showInformation(this,tr("记录导出"),tr("已经将所有数据导出"),tr("关闭"));
}

void RecordQuery::slotBtnFilter()
{
    DataFilter dataFileter(this);
    connect(&dataFileter,&DataFilter::sigSqlQuery,this,&RecordQuery::slotQueryStr);
    dataFileter.exec();
}



