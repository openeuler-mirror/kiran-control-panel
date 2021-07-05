#include "choose-widget.h"
#include "ui_choose-widget.h"
#include <QMouseEvent>
#include <iostream>
#include <QStyleOption>
#include <QPainter>
ChooseWidget::ChooseWidget(QString title, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChooseWidget)
{
    ui->setupUi(this);
    ui->label_text->setText(title);
    ui->label_arrow->setFixedSize(16,16);
    ui->label_arrow->setPixmap(QPixmap(":/images/select.svg"));
    ui->label_name->setStyleSheet("#label_name{color: #919191;}");
    setStyleSheet("#ChooseWidget{background-color: #2d2d2d;border-radius : 6px;}");
}

ChooseWidget::~ChooseWidget()
{
    delete ui;
}

void ChooseWidget::setName(QString name)
{
    ui->label_name->setText(name);
}

void ChooseWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}

void ChooseWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
