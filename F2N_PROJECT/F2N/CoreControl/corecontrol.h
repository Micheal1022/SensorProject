#ifndef CORECONTROL_H
#define CORECONTROL_H

#include <QObject>
class MainWindow;
class CoreControl : public QObject
{
    Q_OBJECT
public:
    explicit CoreControl(QObject *parent = NULL);
    ~CoreControl();
    void initObj();
signals:

private:
    MainWindow *m_mainWindow;

public slots:
};

#endif // CORECONTROL_H
