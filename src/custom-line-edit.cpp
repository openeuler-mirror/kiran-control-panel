#include "custom-line-edit.h"
#include <QInputMethodEvent>
#include <QPainter>
#include <QStyleOption>
#include "ui_custom-line-edit.h"
CustomLineEdit::CustomLineEdit(QWidget *parent) : QLineEdit(parent),
                                                  ui(new Ui::CustomLineEdit)
{
    ui->setupUi(this);
    initUI();
}

CustomLineEdit::~CustomLineEdit()
{
    delete ui;
}

void CustomLineEdit::setIcon(QString icon)
{
    ui->icon->setStyleSheet(QString("QLabel#icon{background-image: url(%1);}").arg(icon));
    QLayout *layout = this->layout();
    layout->setSpacing(10);
}

void CustomLineEdit::hideIcon()
{
    ui->icon->hide();
}

void CustomLineEdit::hideButton()
{
    ui->button->hide();
}

void CustomLineEdit::setPlaceholderText(QString text)
{
    ui->lineEdit->setPlaceholderText(text);
}

void CustomLineEdit::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;

    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}

void CustomLineEdit::mousePressEvent(QMouseEvent *event)
{
    this->setFocus();
    QWidget::mousePressEvent(event);
}

void CustomLineEdit::keyPressEvent(QKeyEvent *event)
{
    //if(event->)
}

void CustomLineEdit::inputMethodEvent(QInputMethodEvent *event)
{
    // ui->lineEdit->setText(event->commitString());
}

void CustomLineEdit::initUI()
{
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocusProxy(ui->lineEdit);
    this->setAttribute(Qt::WA_InputMethodEnabled, true);
    //ui->lineEdit->setFocusPolicy(Qt::NoFocus);
}
