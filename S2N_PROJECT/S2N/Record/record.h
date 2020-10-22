#ifndef RECORD_H
#define RECORD_H

#include <QWidget>

namespace Ui {
class Record;
}
class DataFilter;
class QTableWidget;
class Record : public QWidget
{
    Q_OBJECT

public:
    explicit Record(QWidget *parent = 0);
    ~Record();
    void confShow(int userType);

private:
    Ui::Record *ui;
    QString m_queryStr;
    QString m_formatTime;
    DataFilter *m_dataFilter;

private:
    void initWidget();
    void initConnect();
    QString updateTime(QString dateTimeStr);
    void initTableWidget(QTableWidget *tableWidget);
    void showQuery(QString queryStr,QTableWidget *tableWidget);
signals:
    void sigNodeView();
private slots:
    void slotBtnQuery();
    void slotBtnPrint();
    void slotBtnDelete();
    void slotBtnFilter();
    void slotBtnNodeView();
    void slotConfQueryStr(QString queryStr);
};

#endif // RECORD_H
