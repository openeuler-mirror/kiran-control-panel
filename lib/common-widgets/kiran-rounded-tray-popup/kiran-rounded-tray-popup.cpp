//
// Created by liuxinhao on 2022/6/24.
//

#include "kiran-rounded-tray-popup.h"
#include <QBoxLayout>
#include <QGraphicsDropShadowEffect>
#include "kiran-rounded-window-frame.h"

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

void KiranRoundedTrayPopup::setContentWidget(QWidget *widget)
{
    m_contentLayout->addWidget(widget);
}

void KiranRoundedTrayPopup::init()
{
    auto shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setBlurRadius(15);
    shadowEffect->setOffset(0);
    shadowEffect->setColor("#111111");
    setGraphicsEffect(shadowEffect);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(8);

    auto roundedFrame = new KiranRoundedWindowFrame(this);
    m_contentLayout = new QVBoxLayout(roundedFrame);
    m_contentLayout->setSpacing(0);
    m_contentLayout->setMargin(0);

    layout->addWidget(roundedFrame);
}
