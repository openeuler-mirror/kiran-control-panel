/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "kiran-module-class-widget.h"
#include "ui_kiran-module-class-widget.h"
#include "qmath.h"
#include <QPainter>
#include <QPropertyAnimation>

KiranModuleClassWidget::KiranModuleClassWidget(QWidget *parent) :
    QWidget(parent),m_showShadow(false),
    ui(new Ui::KiranModuleClassWidget)
{
    ui->setupUi(this);
    installEventFilter(this);
    setAttribute(Qt::WA_Hover,true);
    m_listWidget = ui->list_widget;
    //阴影宽度
    m_showShadowWidth = 10;
    this->layout()->setContentsMargins(0, 0, m_showShadowWidth, 0);
    resize(iconModeWd(), height());
}

KiranModuleClassWidget::~KiranModuleClassWidget()
{
    delete ui;
}

void KiranModuleClassWidget::paintEvent(QPaintEvent *event)
{
    if(m_showShadow)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        QRectF rect(10, 10, this->width()-20, this->height()-20);
        path.addRoundRect(rect, 8, 8);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.fillPath(path, QBrush(Qt::white));

        QColor color(0, 0, 0, 50);
        for(int i = 0; i < 10; i++) {
            QPainterPath path;
            path.setFillRule(Qt::WindingFill);
            path.addRect(10-i, 10-i, this->width()-(10-i)*2, this->height()-(10-i)*2);
            color.setAlpha(150 - qSqrt(i)*50);
            painter.setPen(color);
            painter.drawPath(path);
        }
    }

    QWidget::paintEvent(event);
}

bool KiranModuleClassWidget::eventFilter(QObject * obj, QEvent * event)
{
    if(Q_LIKELY(obj == this))
    {
        switch (event->type()) {
        case QEvent::HoverEnter:
            setIconMode(false);
            break;
        case QEvent::HoverLeave:
            setIconMode(true);
            break;
        default:
            break;
        }
    }

    return QWidget::eventFilter(obj, event);
}

void KiranModuleClassWidget::setIconMode(const bool &iconMode)
{
    m_listWidget->setIconMode(iconMode);
    //展开，缩回动画。
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(iconMode ? 100 : 300 );
    animation->setStartValue(QRect(0, 0, width(), this->height()));
    animation->setEndValue(QRect(0, 0, iconMode ? iconModeWd() : textModeWd(), this->height()));
    animation->setEasingCurve(iconMode ? QEasingCurve::InExpo : QEasingCurve::OutExpo);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    //控制文字、阴影隐藏的先后顺序
    if(iconMode)
    {
        connect(animation, &QPropertyAnimation::finished, this, [=](){
            m_listWidget->setTextShow(false);
            m_showShadow = false;
            update();
        });
    }
    else
    {
        m_listWidget->setTextShow(true);
        m_showShadow = true;
        update();
    }
}

/*!
 * \brief KiranModuleClassListWidget::iconModeWd listWidget的总宽度,包括item的宽度+item的Space属性的宽度+listWidget的padding宽度.
 * \return
 */
int KiranModuleClassWidget::iconModeWd()
{
    return 96+m_showShadowWidth+1;//加1个像素的右边距.
}

int KiranModuleClassWidget::textModeWd()
{
    return 252;
}
