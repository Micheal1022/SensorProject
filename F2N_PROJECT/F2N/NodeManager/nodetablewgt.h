#ifndef NODETABLEWDT_H
#define NODETABLEWDT_H

#include <QWidget>

namespace Ui {
class NodeTableWgt;
}
class QTableWidget;
class QButtonGroup;
class NodeTableWgt : public QWidget
{
    Q_OBJECT

public:
    explicit NodeTableWgt(QWidget *parent = 0);
    ~NodeTableWgt();
    void confShow(QString pass);

private:
    Ui::NodeTableWgt *ui;
    int m_nodeCount;
    QString m_pass;
    QButtonGroup *m_btnKeyGroup;
    QList<QStringList> m_nodeInfoList;
    void initKey();
    void initConnect();
    void confTableItem(QTableWidget *tableWidget,QList<QStringList> nodeInfoList);
    void initTableWidget(QTableWidget *tableWidget);

private slots:
    void slotBtnOk();
    void slotBtnDel();
    void slotBtnSaveConf();
    void slotBtnClick(int index);
    void slotCellDoubleClicked(int row, int column);
};

#endif // NODETABLEWDT_H
