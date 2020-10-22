#ifndef SELFCHECK_H
#define SELFCHECK_H

#include <QWidget>

namespace Ui {
class SelfCheck;
}
class QTimer;
class SelfCheck : public QWidget
{
    Q_OBJECT

public:
    explicit SelfCheck(QWidget *parent = 0);
    ~SelfCheck();
    void selfCheckStart();

private:
    Ui::SelfCheck *ui;
    QTimer* m_colorTimer;
    QString m_redStr;
    QString m_blueStr;
    QString m_greenStr;
    QString m_whiteStr;
    QString m_yellowStr;
    int m_colorType;
signals:
    void sigSelfCheckFinish();
    void sigControlLight(uchar data_1, uchar data_2);
    void sigControlSound(int type);
private slots:
    void slotColorTimeOut();
public slots:
};

#endif // SELFCHECK_H
