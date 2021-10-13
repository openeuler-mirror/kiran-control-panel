#include "custom-line-edit.h"
#include <kiran-log/qt5-log-i.h>
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
    setFixedHeight(40);
}

void CustomLineEdit::keyReleaseEvent(QKeyEvent *event)
{
    QList<int> keycodes;

    if (event->key() == Qt::Key_Backspace && event->modifiers() == Qt::NoModifier)
    {
        return;
    }
    // no modifier
    if (event->key() != 0 && event->modifiers() == Qt::NoModifier)
    {
        if (event->key() == Qt::Key_Shift || event->key() == Qt::Key_Control || event->key() == Qt::Key_Alt)
            return;
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
    else if (event->key() != 0 && event->modifiers() == (Qt::AltModifier | Qt::ShiftModifier | Qt::ControlModifier))
    {
        keycodes.append(Qt::Key_Shift);
        keycodes.append(Qt::Key_Control);
        keycodes.append(Qt::Key_Alt);
        keycodes.append(event->key());
    }
    if (keycodes.size() > 0)
    {
        emit inputKeyCodes(keycodes);
    }
}
