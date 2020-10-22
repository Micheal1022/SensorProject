#ifndef RECORDQUERY_H
#define RECORDQUERY_H

#include <QWidget>
#include <GlobalData/globaldata.h>
namespace Ui {
class RecordQuery;
}
class DataFilter;
class QTableWidget;
class RecordQuery : public QWidget,public GlobalData
{
    Q_OBJECT

public:
    explicit RecordQuery(QWidget *parent = 0);
    ~RecordQuery();
    void confShow(UserType user);
private:
    Ui::RecordQuery *ui;
    QString m_queryStr;
    QString m_formatTime;
    DataFilter *m_dataFilter;

    void initConnect();
    void initTableWidget(QTableWidget *tableWidget);
    void initWidget();
    void showQuery(QString queryStr,QTableWidget *tableWidget);
signals:
    void sigNodeInfo();
private slots:
    void slotBtnBack();
    void slotBtnQuery();
    void slotQueryStr(QString queryStr);
    void slotBtnPrint();
    void slotBtnFilter();
    void slotBtnDelete();
    void slotBtnExport();
};

#endif // RECORDQUERY_H
