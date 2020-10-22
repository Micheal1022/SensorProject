#ifndef USERLOGIN_H
#define USERLOGIN_H

#include <QWidget>

/*
* @projectName   SS_FPME_V2
* @brief         摘要
* @author        Micheal
* @date          2019-01-16
*/

namespace Ui {
class UserLogin;
}
class QButtonGroup;
class UserLogin : public QWidget
{
    Q_OBJECT

public:
    explicit UserLogin(QWidget *parent = 0);
    ~UserLogin();
    void confShow();

signals:
    void sigNodeInfo();
    void sigLogin();
private slots:
    void slotBtnOk();
    void slotBtnBack();
    void slotBtnDel();
    void slotBtnClick(int index);

private:
    Ui::UserLogin *ui;
    void initKey();
    QButtonGroup *m_btnKeyGroup;
};

#endif // USERLOGIN_H
