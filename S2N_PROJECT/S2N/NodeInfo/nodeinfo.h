#ifndef NODEINFO_H
#define NODEINFO_H

#include <QWidget>

namespace Ui {
class NodeInfo;
}
class QButtonGroup;
class NodeInfo : public QWidget
{
    Q_OBJECT

public:
    explicit NodeInfo(QWidget *parent = 0);
    ~NodeInfo();
    void confShow();
private:
    Ui::NodeInfo *ui;
    QButtonGroup *m_btnKeyGroup;
    void initKey();
    void setAlarmData();
signals:
    void sigNodeView();
    void sigSendAlarmData(int pass, quint32 ID, quint16 type, quint16 alarm, quint16 base);
private slots:
    void slotBtnClick(int index);

};

#endif // NODEINFO_H
