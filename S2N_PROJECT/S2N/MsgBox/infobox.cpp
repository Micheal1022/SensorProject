#include "infobox.h"
#include "ui_infobox.h"
#include <QBitmap>
#include <QPainter>

InfoBox::InfoBox(QWidget *parent, const QString &title, const QString &context,const QString &btn) :
    QDialog(parent),
    ui(new Ui::InfoBox)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    //设置关于窗体为圆角
    QBitmap bmp(this->size());
    bmp.fill();
    QPainter p(&bmp);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.drawRoundedRect(bmp.rect(),4,4);
    setMask(bmp);

    ui->lb_title->setText(title);
    ui->lb_context->setText(context);
    ui->pBtn_ok->setText(btn);

    connect(ui->pBtn_ok,&QToolButton::clicked,this,&InfoBox::close);
}

InfoBox::~InfoBox()
{
    delete ui;
}

void InfoBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QPen pen;
    pen.setColor(QColor(200, 200, 200));
    pen.setWidth(1);
    pen.setStyle( Qt::SolidLine);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawRoundedRect(0, 0, this->width(), this->height(), 4.0,4.0, Qt::AbsoluteSize);
}

