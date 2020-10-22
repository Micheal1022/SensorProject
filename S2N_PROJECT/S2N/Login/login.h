#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#define USER    0
#define ADMIN   1
#define SUPER   2
namespace Ui {
class Login;
}
class QButtonGroup;
class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();
    void confShow();
private:
    Ui::Login *ui;
    void initKey();
    QButtonGroup *m_btnKeyGroup;

signals:
    void sigNodeView();
    void sigLoginOk(int userType);
private slots:
    void slotBtnOk();
    void slotBtnBackNodeView();
    void slotBtnClear();
    void slotBtnClick(int index);


};

#endif // LOGIN_H
