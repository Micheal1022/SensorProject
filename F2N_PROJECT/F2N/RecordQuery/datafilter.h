#ifndef DATAFILTER_H
#define DATAFILTER_H

#include <QDialog>

namespace Ui {
class DataFilter;
}
class QButtonGroup;
class DataFilter : public QDialog
{
    Q_OBJECT

public:
    explicit DataFilter(QWidget *parent = 0);
    ~DataFilter();

private:
    Ui::DataFilter *ui;
    QButtonGroup *m_btnKeyGroup;

    void initKey();
    void initWidget();
    void initConnect();
signals:
    void sigSqlQuery(QString queryStr);

private slots:
    void slotBtnOk();
    void slotBtnBack();
    void slotBtnDel();
    void slotBtnHostError();
    void slotBtnClick(int index);
    void slotSetCanIdEnable(bool enable);
    void slotSetPassEnable(bool enable);
};

#endif // DATAFILTER_H
