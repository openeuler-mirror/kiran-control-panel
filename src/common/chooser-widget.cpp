#include "chooser-widget.h"
#include "ui_chooser-widget.h"
#include "common/include.h"
#include <QMouseEvent>
#include <QStyleOption>
#include <QPainter>

ChooserWidget::ChooserWidget(QString title, int type, QWidget *parent):
    QWidget(parent),
    ui(new Ui::ChooserWidget)
{
    ui->setupUi(this);
    initUI(title);
    m_wallpaperType = type;
}

ChooserWidget::~ChooserWidget()
{
    delete ui;
}

/**
 * @brief ChooserWidget::setName 设置选择控件名字
 * @param name 名字
 */
void ChooserWidget::setName(QString name)
{
    ui->label_name->setText(name);
}

void ChooserWidget::initUI(QString title)
{
    ui->label_text->setText(title);
    ui->label_arrow->setFixedSize(16,16);
    ui->label_arrow->setPixmap(QPixmap(":/images/select.svg"));
    ui->label_name->setStyleSheet("#label_name{color: #919191;}");
    setStyleSheet("#ChooserWidget{background-color: #2d2d2d;border-radius : 6px;}");
}

void ChooserWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}

void ChooserWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
