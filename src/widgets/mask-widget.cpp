//
// Created by lxh on 2020/11/5.
//

#include "mask-widget.h"

#include <QEvent>
#include <QResizeEvent>

MaskWidget::MaskWidget(QWidget *parent)
    : QWidget(parent),
      m_maskedWidget(parent)
{
    m_maskedWidget->installEventFilter(this);
}

bool MaskWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_maskedWidget)
    {
        switch (event->type())
        {
        case QEvent::Resize:
        {
            auto resizeEvent = dynamic_cast<QResizeEvent *>(event);
            //TODO:后续添加功能,是否不遮住自定义标题栏
            resize(resizeEvent->size());
            break;
        }
        default:
            break;
        }
    }
    return QObject::eventFilter(watched, event);
}
