#ifndef STATEINFO_H
#define STATEINFO_H

#include <QDialog>
#include <GlobalData/globaldata.h>

namespace Ui {
class StateInfo;
}


#define MOD_LEAK 2
#define MOD_TEMP 3

#define MOD_ALARM 1
#define MOD_ERROR 2
#define MOD_DROP  3

#define PASS  0
#define CANID 1
#define STS   2
#define VALUE 3
#define TIME  4
#define ADDR  5
class StateInfo : public QDialog,public GlobalData
{
    Q_OBJECT

public:
    explicit StateInfo(QWidget *parent = 0);
    ~StateInfo();
    void initConf(QList<QStringList> dataList,DataType type);
    void initConnect();

private:
    Ui::StateInfo *ui;
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

#endif // STATEINFO_H
