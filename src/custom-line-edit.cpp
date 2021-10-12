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
    QStringList keys;
    // no modifier
    if (event->key() != 0 && event->modifiers() == Qt::NoModifier)
    {
        keycodes.append(event->key());
        keys.append(QKeySequence(event->key()).toString());
        //KLOG_INFO() << QKeySequence(event->key()).toString();
    }
    // one modifier
    else if (event->key() != 0 && event->modifiers() == Qt::ShiftModifier)
    {
        keycodes.append(Qt::Key_Shift);
        keycodes.append(event->key());
        keys.append(QKeySequence(event->modifiers() + event->key()).toString());
    }
    else if (event->key() != 0 && event->modifiers() == Qt::ControlModifier)
    {
        keycodes.append(Qt::Key_Control);
        keycodes.append(event->key());
        keys.append(QKeySequence(event->modifiers() + event->key()).toString());
    }
    else if (event->key() != 0 && event->modifiers() == Qt::AltModifier)
    {
        keycodes.append(Qt::Key_Alt);
        keycodes.append(event->key());
        keys.append(QKeySequence(event->modifiers() + event->key()).toString());
    }
    // two modifier
    else if (event->key() != 0 && event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
    {
        keycodes.append(Qt::Key_Control);
        keycodes.append(Qt::Key_Shift);
        keycodes.append(event->key());
        keys.append(QKeySequence(event->modifiers() + event->key()).toString());
    }
    else if (event->key() != 0 && event->modifiers() == (Qt::ControlModifier | Qt::AltModifier))
    {
        keycodes.append(Qt::Key_Control);
        keycodes.append(Qt::Key_Alt);
        keycodes.append(event->key());
        keys.append(QKeySequence(event->modifiers() + event->key()).toString());
    }
    else if (event->key() != 0 && event->modifiers() == (Qt::ShiftModifier | Qt::AltModifier))
    {
        keycodes.append(Qt::ShiftModifier);
        keycodes.append(Qt::Key_Alt);
        keycodes.append(event->key());
        keys.append(QKeySequence(event->modifiers() + event->key()).toString());
    }
    //three modifier
    else if (event->key() != 0 && event->modifiers() == (Qt::ShiftModifier | Qt::AltModifier | Qt::ControlModifier))
    {
        keycodes.append(Qt::Key_Control);
        keycodes.append(Qt::Key_Alt);
        keycodes.append(Qt::ShiftModifier);
        keycodes.append(event->key());
        keys.append(QKeySequence(event->modifiers() + event->key()).toString());
    }

    //    if (keys.size() > 0)
    //    {
    //        foreach (QString key, keys)
    //        {
    //            KLOG_INFO() << key;
    //        }
    //    }
    if (keycodes.size() > 0)
    {
        emit inputKeyCodes(keycodes);
    }
}
