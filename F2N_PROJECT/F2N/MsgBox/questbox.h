#ifndef QUESTBOX_H
#define QUESTBOX_H

#include <QDialog>
#include <QButtonGroup>


namespace Ui {
class QuestBox;
}

class QuestBox : public QDialog
{
    Q_OBJECT

public:
    explicit QuestBox(QWidget *parent = 0,const QString &title = QString("提示"),const QString &context = QString("内容"),
                      const QString &btn1 = QString("确定"),const QString &btn2 = QString("取消"));
    ~QuestBox();
    int btnResult();
    int m_index;

private:
    Ui::QuestBox *ui;
    QButtonGroup *m_btnGroup;

public slots:
    void slot_btnClick(int index);

protected:
    void paintEvent(QPaintEvent * event);

};

#endif // QUESTBOX_H
