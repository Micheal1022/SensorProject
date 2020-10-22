#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class Login;
class QTimer;
class Record;
class MainPage;
class NodeInfo;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Login *m_login;
    Record *m_record;
    NodeInfo *m_nodeInfo;
    MainPage *m_mainPage;

    QTimer *m_systemTime;
    int m_userType;
    void initConncet();
    void initWidget();
private slots:
    void slotBtnLogin();
    void slotBtnRecord();
    void slotSystemTime();
    void slotBtnNodeView();
    void slotBtnNodeInfo();
    void slotLoginOk(int userType);
};

#endif // MAINWINDOW_H
