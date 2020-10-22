#include "questbox.h"
#include "ui_questbox.h"
#include <QBitmap>
#include <QPainter>

QuestBox::QuestBox(QWidget *parent, const QString &title, const QString &context,
                   const QString &btn1, const QString &btn2) :
    QDialog(parent),
    ui(new Ui::QuestBox)
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

    m_btnGroup = new QButtonGroup;
    m_btnGroup->addButton(ui->pBtn_ok,0);
    m_btnGroup->addButton(ui->pBtn_cancel,1);

    ui->lb_title->setText(title);
    ui->lb_context->setText(context);
    ui->pBtn_ok->setText(btn1);
    ui->pBtn_cancel->setText(btn2);

    connect(m_btnGroup,SIGNAL(buttonClicked(int)),this,SLOT(slot_btnClick(int)));
}

QuestBox::~QuestBox()
{
    delete ui;
}

int QuestBox::btnResult()
{
    return m_index;
}

void QuestBox::slot_btnClick(int index)
{
    this->m_index = index;
    done(index);
}

void QuestBox::paintEvent(QPaintEvent *event)
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
