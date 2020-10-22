#ifndef INFOBOX_H
#define INFOBOX_H

#include <QDialog>

namespace Ui {
class InfoBox;
}

class  InfoBox : public QDialog
{
    Q_OBJECT

public:
    explicit InfoBox(QWidget *parent = 0,const QString &title = QString("提示"),const QString &context = QString("内容"),
                     const QString &btn = QString("确定"));
    ~InfoBox();

private:
    Ui::InfoBox *ui;

protected:
    void paintEvent(QPaintEvent * event);


};

#endif // INFOBOX_H
