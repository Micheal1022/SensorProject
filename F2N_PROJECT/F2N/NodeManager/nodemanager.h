#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include <QWidget>

namespace Ui {
class NodeManager;
}
class QTableWidget;
class NodeTableWgt;
class NodeManager : public QWidget
{
    Q_OBJECT

public:
    explicit NodeManager(QWidget *parent = 0);
    ~NodeManager();
    void confShow();

private:
    Ui::NodeManager *ui;
    NodeTableWgt *nodeTableWgt_1;
    NodeTableWgt *nodeTableWgt_2;
    void initConnect();

signals:
    void sigNodeInfo();
    void sigSaveOk();
private slots:
    void slotBtnEffect();

};

#endif // NODEMANAGER_H
