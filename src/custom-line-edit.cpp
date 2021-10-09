#include "custom-line-edit.h"
#include <QInputMethodEvent>
#include <QPainter>
#include <QStyleOption>

CustomLineEdit::CustomLineEdit(QWidget *parent) : QLineEdit(parent)
{
    initUI();
}

CustomLineEdit::~CustomLineEdit()
{
}

void CustomLineEdit::initUI()
{
    //    this->setFocusPolicy(Qt::StrongFocus);
}

void CustomLineEdit::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;

    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}

void CustomLineEdit::keyReleaseEvent(QKeyEvent *event)
{
    QList<int> keycodes;
    // NoModifier
    if (event->key() != 0 && event->modifiers() == Qt::NoModifier)
    {
        keycodes.append(event->key());
    }
    // one modifier
    else if (event->key() != 0 && event->modifiers() == Qt::ShiftModifier)
    {
        keycodes.append(Qt::Key_Shift);
        keycodes.append(event->key());
    }
    else if (event->key() != 0 && event->modifiers() == Qt::ControlModifier)
    {
        keycodes.append(Qt::Key_Control);
        keycodes.append(event->key());
    }
    else if (event->key() != 0 && event->modifiers() == Qt::AltModifier)
    {
        keycodes.append(Qt::Key_Alt);
        keycodes.append(event->key());
    }
    // two modifier
    else if (event->key() != 0 && event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
    {
        keycodes.append(Qt::Key_Control);
        keycodes.append(Qt::Key_Shift);
        keycodes.append(event->key());
    }
    else if (event->key() != 0 && event->modifiers() == (Qt::ControlModifier | Qt::AltModifier))
    {
        keycodes.append(Qt::Key_Control);
        keycodes.append(Qt::Key_Alt);
        keycodes.append(event->key());
    }
    else if (event->key() != 0 && event->modifiers() == (Qt::ShiftModifier | Qt::AltModifier))
    {
        keycodes.append(Qt::ShiftModifier);
        keycodes.append(Qt::Key_Alt);
        keycodes.append(event->key());
    }
    //three modifier
    else if (event->key() != 0 && event->modifiers() == (Qt::ShiftModifier | Qt::AltModifier | Qt::ControlModifier))
    {
        keycodes.append(Qt::Key_Control);
        keycodes.append(Qt::Key_Alt);
        keycodes.append(Qt::ShiftModifier);
        keycodes.append(event->key());
    }
    if (keycodes.size() > 0)
    {
        emit inputKeyCodes(keycodes);
    }
}
