#include "record.h"
#include "ui_record.h"
#include <QDateTime>
#include <QDebug>
#include <QTextEdit>

#include "datafilter.h"
#include "SQLite/sqlite.h"
#include "MsgBox/msgbox.h"
#include "MyPrint/myprint.h"
#define USER    0
#define ADMIN   1
#define SUPER   2


#define PASS    0
#define CANID   1
#define TYPE    2
#define STATE   3
#define VALUE   4
#define TIME    5
#define AREA    6

#define HOST    1
#define LEAK    2
#define TEMP    3
#define EARC    4

#define ALARM   1
#define ERROR   2
#define OFFLINE 3
#define MPERROR 4
#define BPERROR 5

Record::Record(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Record)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    initWidget();
    initConnect();
    initTableWidget(ui->tableWgtRecord);
}

Record::~Record()
{
    delete ui;
}

void Record::confShow(int userType)
{
    switch (userType) {
    case USER:
        ui->tBtnDel->setVisible(false);
        ui->tBtnPrint->setVisible(false);
        break;
    case ADMIN:
        ui->tBtnDel->setVisible(false);
        ui->tBtnPrint->setVisible(true);
        break;
    case SUPER:
        ui->tBtnDel->setVisible(true);
        ui->tBtnPrint->setVisible(true);
        break;
    }
    QString dateTimeStr = QString::number(QDateTime::currentDateTime().toTime_t());
    m_queryStr = "select PASS,CANID,TYPE,STATE,VALUE,TIME,AREA from RECORD where TIME <= "+dateTimeStr+" order by TIME desc;";
}

void Record::initConnect()
{
    connect(ui->tBtnPrint, &QToolButton::clicked,this,&Record::slotBtnPrint);
    connect(ui->tBtnQuery, &QToolButton::clicked,this,&Record::slotBtnQuery);
    connect(ui->tBtnDel,   &QToolButton::clicked,this,&Record::slotBtnDelete);
    connect(ui->tBtnFilter,&QToolButton::clicked,this,&Record::slotBtnFilter);
    connect(m_dataFilter,&DataFilter::sigSqlQuery,this,&Record::slotConfQueryStr);
    connect(ui->tBtnBackNodeView,&QToolButton::clicked,this,&Record::slotBtnNodeView);
}


void Record::initTableWidget(QTableWidget *tableWidget)
{
    QStringList headList;
    headList<<tr("通道")<<tr("地址")<<tr("类型")<<tr("状态")<<tr("数值")<<tr("时间")<<tr("区域");
    tableWidget->setColumnCount(headList.size());
    tableWidget->setHorizontalHeaderLabels(headList);
    tableWidget->horizontalHeader()->setFixedHeight(30);
    tableWidget->horizontalHeader()->setDefaultSectionSize(140);
    QString styleStr = "QHeaderView::section{font: 13pt '文泉驿等宽微米黑'; background-color: rgb(28, 183, 200);color: black;}";
    QString vstyleStr ="QHeaderView::section{font: 12pt '文泉驿等宽微米黑';background-color: rgb(15, 41, 68);color: white;}";
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

    tableWidget->setColumnWidth(0, 80); //通道
    tableWidget->setColumnWidth(1, 80); //地址
    tableWidget->setColumnWidth(2, 80); //类型
    tableWidget->setColumnWidth(3, 90); //状态
    tableWidget->setColumnWidth(4, 90); //数值
    tableWidget->setColumnWidth(5,250); //时间
    tableWidget->setColumnWidth(6,250); //区域
}

void Record::initWidget()
{
    m_dataFilter = new DataFilter;

    QSettings settings(QCoreApplication::applicationDirPath()+"/Conf.ini", QSettings::IniFormat);
    QString printName = settings.value("PRINT/PrintName").toString();

    MyPrint::initSerialPort(printName);
    m_formatTime = QString("yyyy-MM-dd hh:mm:ss");
    ui->lbCountNum->setText("0");
    QString dateTimeStr = QString::number(QDateTime::currentDateTime().toTime_t());
    m_queryStr = "select PASS,CANID,TYPE,STATE,VALUE,TIME,AREA from RECORD where TIME <= "+dateTimeStr+" order by TIME desc;";
}

void Record::showQuery(QString queryStr, QTableWidget *tableWidget)
{
    tableWidget->clearContents();
    QSqlDatabase db = SQLite::openConnection();
    QString sqlCount = "select count(*) "+queryStr.right(queryStr.size() - queryStr.indexOf("from"));
    int rowCount = SQLite::getRowCount(db,sqlCount);
    if (rowCount <= 0) {
        tableWidget->setRowCount(0);
        SQLite::closeConnection(db);
        MsgBox::showInformation(this,tr("查询提示"),tr("没有符合条件的记录!"),tr("关闭"));
        return;
    }
    tableWidget->setRowCount(rowCount);
    ui->lbCountNum->setText(QString::number(rowCount));
    QFont ft("Noto Sans [monotype]",13);
    QTableWidgetItem *item;
    QSqlQuery query(db);
    //qDebug()<<"queryStr : "<<queryStr;
    if (query.exec(queryStr)) {
        int row = 0;
        tableWidget->setRowHeight(row,25);
        while (query.next()) {
            QDateTime dateTime;
            for (int column = 0;column < tableWidget->columnCount(); column++) {
                item = new QTableWidgetItem;
                item->setFont(ft);
                item->setTextColor(QColor(255,255,255));
                item->setTextAlignment(Qt::AlignCenter);

                switch (column) {
                case PASS:
                    item->setText(query.value(column).toString());
                    break;
                case CANID:
                    item->setText(query.value(column).toString());
                    break;
                case TYPE:
                    switch (query.value(column).toInt()) {
                    case HOST:
                        item->setText(tr("主机"));
                        break;
                    case LEAK:
                        item->setText(tr("漏电"));
                        break;
                    case TEMP:
                        item->setText(tr("温度"));
                        break;
                    case EARC:
                        item->setText(tr("电弧"));
                        break;
                    }
                    break;
                case STATE:
                    switch (query.value(column).toInt()) {
                    case ALARM:
                        item->setText(tr("节点报警"));
                        break;
                    case ERROR:
                        item->setText(tr("节点故障"));
                        break;
                    case OFFLINE:
                        item->setText(tr("通讯故障"));
                        break;
                    case MPERROR:
                        item->setText(tr("主电故障"));
                        break;
                    case BPERROR:
                        item->setText(tr("备电故障"));
                        break;
                    }
                    break;
                case VALUE:
                    if (LEAK == query.value(TYPE).toInt() && ALARM == query.value(STATE).toInt()) {
                        item->setText(query.value(column).toString()+tr("mA"));
                    } else if (TEMP == query.value(TYPE).toInt() && ALARM == query.value(STATE).toInt()) {
                        item->setText(query.value(column).toString()+tr("℃"));
                    } else {
                        item->setText("   ");
                    }
                    break;
                case TIME:
                    dateTime.setTime_t(query.value(column).toUInt());
                    item->setText(dateTime.toString(m_formatTime));
                    break;
                case AREA:
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

void Record::slotBtnQuery()
{
    showQuery(m_queryStr,ui->tableWgtRecord);
}

void Record::slotConfQueryStr(QString queryStr)
{
    m_queryStr = queryStr;
    showQuery(m_queryStr,ui->tableWgtRecord);
}

void Record::slotBtnPrint()
{
    if (ui->tableWgtRecord->currentRow() < 0) {
        MsgBox::showInformation(this,tr("打印提示"),tr("未选中要打印的内容!"),tr("关闭"));
        return;
    }

    for (int row = 0;row < ui->tableWgtRecord->rowCount();row++) {
        if (ui->tableWgtRecord->item(row,1)->isSelected()) {
            QString pass  = ui->tableWgtRecord->item(row,0)->text();
            QString canId = ui->tableWgtRecord->item(row,1)->text();
            QString type  = ui->tableWgtRecord->item(row,2)->text();
            QString state = ui->tableWgtRecord->item(row,3)->text();
            QString value = ui->tableWgtRecord->item(row,4)->text();
            QString time  = ui->tableWgtRecord->item(row,5)->text();
            QString area  = ui->tableWgtRecord->item(row,6)->text();
            qDebug()<<pass<<canId<<type<<state<<value<<time<<area;
            MyPrint::dataPring(pass,canId,type,state,value,time,area);
        }
    }
}

void Record::slotBtnDelete()
{
    int ret = MsgBox::showQuestion(this,tr("删除提示"),tr("您确定要删除数据?"),tr("部分"),tr("所有"));
    if (ret == 0) {
        for (int row = 0;row < ui->tableWgtRecord->rowCount();row++) {
            if (ui->tableWgtRecord->item(row,1)->isSelected()) {
                int pass  = ui->tableWgtRecord->item(row,PASS)->text().toUInt();
                int canId = ui->tableWgtRecord->item(row,CANID)->text().toUInt();
                QString nodeState = ui->tableWgtRecord->item(row,STATE)->text();
                int state = 0;
                if (nodeState == tr("节点报警")) {
                    state = ALARM;
                }
                else if (nodeState == tr("节点故障")) {
                    state = ERROR;
                }
                else if (nodeState == tr("通讯故障")) {
                    state = OFFLINE;
                }
                else if (nodeState == tr("主电故障")) {
                    state = MPERROR;
                }
                else if (nodeState == tr("备电故障")) {
                    state = BPERROR;
                }

                uint alarmTime = QDateTime::fromString(ui->tableWgtRecord->item(row,TIME)->text(),m_formatTime).toTime_t();
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

void Record::slotBtnFilter()
{
    m_dataFilter->show();
}

void Record::slotBtnNodeView()
{
    emit sigNodeView();
}
