//
// Created by liuxinhao on 2022/6/24.
//

#include "kiran-rounded-tray-popup.h"
#include "kiran-rounded-window-frame.h"

#include <QBitmap>
#include <QBoxLayout>
#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>

KiranRoundedTrayPopup::KiranRoundedTrayPopup(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::Popup | Qt::BypassWindowManagerHint);
    init();
}

KiranRoundedTrayPopup::~KiranRoundedTrayPopup()
{
}

void KiranRoundedTrayPopup::updateContentMask(QWidget *widget)
{
    QBitmap bitMap(widget->size());
    bitMap.fill();

    QPainterPath painterPath;
    QRectF rectf = widget->rect();

    QPainter painter(&bitMap);
    painterPath.addRoundRect(rectf, KiranRoundedWindowFrame::radius, KiranRoundedWindowFrame::radius);
    painter.fillPath(painterPath, Qt::black);
    painter.end();

    widget->setMask(bitMap);
}

bool KiranRoundedTrayPopup::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_contentWidget && event->type() == QEvent::Resize)
    {
        updateContentMask(m_contentWidget);
    }
    return QWidget::eventFilter(watched, event);
}

void KiranRoundedTrayPopup::setContentWidget(QWidget *widget)
{
    if (m_contentWidget != nullptr)
    {
        m_contentWidget->removeEventFilter(this);
        m_contentLayout->removeWidget(m_contentWidget);
        delete m_contentWidget;
        m_contentWidget = nullptr;
    }

    m_contentWidget = widget;
    m_contentWidget->installEventFilter(this);
    m_contentLayout->addWidget(m_contentWidget);
}

void KiranRoundedTrayPopup::init()
{
    auto shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setOffset(0);
    shadowEffect->setColor("#111111");
    setGraphicsEffect(shadowEffect);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(8);
    layout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    auto roundedFrame = new KiranRoundedWindowFrame(this);
    m_contentLayout = new QVBoxLayout(roundedFrame);
    m_contentLayout->setSpacing(0);
    m_contentLayout->setMargin(1);
    m_contentLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    layout->addWidget(roundedFrame);
}