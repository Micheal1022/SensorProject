#ifndef INFOVIEW_H
#define INFOVIEW_H

#include <QDialog>

namespace Ui {
class InfoView;
}

class InfoView : public QDialog
{
    Q_OBJECT

public:
    explicit InfoView(QWidget *parent = 0);
    ~InfoView();
    //数据类型
    enum DataType{
        AlarmData,
        ErrorData
    };
    void initConf(QList<QStringList> dataList,DataType type);
    void initConnect();

private:
    Ui::InfoView *ui;
    int m_curIndex;
    QList<QStringList> m_dataList;
    void infoShow(QStringList stringList);

private slots:
    void slotBtnHeadItem();
    void slotBtnTailItem();
    void slotBtnNestItem();
    void slotBtnLastItem();
    void slotBtnBack();
};

#endif // INFOVIEW_H
