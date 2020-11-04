#include "avatar-item-button.h"

#include <QStyleOption>
#include <QPainter>
#include <QResizeEvent>
#include <QtDebug>
#include <QMouseEvent>
#include <QtMath>
#include <QSvgRenderer>

AvatarItemButton::AvatarItemButton(QWidget *parent)
    : QAbstractButton(parent),
      m_mouseEnter(false),
      m_hover(false)
{
    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);
}

AvatarItemButton::~AvatarItemButton()
{

}

void AvatarItemButton::setIcon(const QString &iconPath)
{
    m_iconPath = iconPath;
    setIcon(QIcon(iconPath));
}

QString AvatarItemButton::iconPath()
{
    return m_iconPath;
}

void AvatarItemButton::setIcon(const QIcon &icon)
{
    QAbstractButton::setIcon(icon);
}

void AvatarItemButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::SmoothPixmapTransform|
                           QPainter::HighQualityAntialiasing|
                           QPainter::Antialiasing);
    if( !icon().isNull() ){
        QPainterPath painterPath;
        QPixmap pixmap = icon().pixmap(QSize(m_radius*2,m_radius*2));
        painterPath.addEllipse((this->width()-m_radius*2)/2,
                               (this->height()-m_radius*2)/2,
                               m_radius*2,
                               m_radius*2);
        painter.setClipPath(painterPath);
        painter.drawPixmap((this->width()-pixmap.width())/2,
                           (this->height()-pixmap.height())/2,
                           pixmap);
    }
    if(m_hover&&!isChecked()){
        painter.save();
        QPen pen;
        pen.setColor(QColor(255,255,255,255*0.5));
        pen.setWidth(3);
        painter.setPen(pen);
        painter.drawEllipse((this->width()-(m_radius*2))/2,(this->height()-(m_radius*2))/2,m_radius*2,m_radius*2);
        painter.restore();
    }
    if(isChecked()){
        painter.save();
        ///遮罩
        QPainterPath painterPath;
        painterPath.addEllipse((this->width()-(m_radius*2))/2,(this->height()-(m_radius*2))/2,m_radius*2,m_radius*2);
        painter.fillPath(painterPath,QBrush(QColor(0,0,0,255*0.6)));

        ///边框
        QPen pen;
        pen.setColor("#12adfb");
        pen.setWidth(3);
        painter.setPen(pen);
        painter.drawEllipse((this->width()-(m_radius*2))/2,(this->height()-(m_radius*2))/2,m_radius*2,m_radius*2);
        painter.restore();

        ///画勾
        QSize checkedIconSize(40,40);
        QPoint checkedIconLeftop((this->width()-checkedIconSize.width())/2,
                                 (this->height()-checkedIconSize.height())/2);
        QRect checkedIconRect(checkedIconLeftop,checkedIconSize);
        QSvgRenderer svgRenderer(QString(":/images/icon_correct.svg"));
        svgRenderer.render(&painter,checkedIconRect);
    }
}

void AvatarItemButton::resizeEvent(QResizeEvent *event)
{
    QRect realRect;
    QSize size = event->size();
    double radius = ((size.width()<size.height())?size.width():size.height())/2.0;
    realRect = QRect((size.width()-(radius*2))/2,
                     (size.height()-(radius*2))/2,
                     radius*2,
                     radius*2);
    m_radius = radius;
    m_realRect = realRect;
}

void AvatarItemButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_mouseEnter = true;
}

void AvatarItemButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_mouseEnter = false;
    m_hover = false;
}

void AvatarItemButton::mouseMoveEvent(QMouseEvent *event)
{
    if( m_mouseEnter ){
        QPoint curMousePos = event->pos();
        QPoint center(this->width()/2,this->height()/2);
        double radius = ((this->width()<this->height())?this->width():this->height())/2.0;
        int distance = qSqrt( qPow(curMousePos.x()-center.x(),2) + qPow(curMousePos.y()-center.y(),2) );
        if( (distance<=radius) && (!m_hover) ){
            m_hover = true;
            repaint();
        }else if( (distance>radius) && m_hover ){
            m_hover = false;
            repaint();
        }
    }
}

void AvatarItemButton::mousePressEvent(QMouseEvent *e)
{
    if( m_hover ){
        QAbstractButton::mousePressEvent(e);
    }
    return ;
}
