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
    setFixedHeight(40);
    setFocusPolicy(Qt::ClickFocus);
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
        //KLOG_INFO() << "shift :" << event->key() << event->modifiers();
    }
    else if (event->key() != 0 && event->modifiers() == Qt::ControlModifier)
    {
        keycodes.append(Qt::Key_Control);
        keycodes.append(event->key());
        //KLOG_INFO() << "ctrl :" << event->key() << event->text();
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
        keycodes.append(Qt::Key_Shift);
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

void CustomLineEdit::focusInEvent(QFocusEvent *e)
{
    Q_UNUSED(e);
    update();
    clear();
    grabKeyboard();
    m_flag = true;
}
void CustomLineEdit::focusOutEvent(QFocusEvent *e)
{
    releaseKeyboard();
    update();
}
