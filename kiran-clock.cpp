#include "kiran-clock.h"

#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QPixmap>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>

#define HOUR_HAND_TAIR_RATIO    0.23
#define MINUTE_HAND_TAIR_RATIO  0.18
#define SECOND_HAND_TAIR_RATIO  0.25

KiranClock::KiranClock(QWidget *parent)
    : QWidget(parent)
{
    loadPixmap();
    initShadow();
    startTimer(50);
}

KiranClock::~KiranClock()
{

}

void KiranClock::loadPixmap()
{
    m_dialReader.load(QString(":/images/dial.svg"));
    m_hourReader.load(QString(":/images/hour_hand.svg"));
    m_minuteReader.load(QString(":/images/minute_hand.svg"));
    m_secondReader.load(QString(":/images/second_hand.svg"));

    update();
}

void KiranClock::initShadow()
{
    m_effect = new QGraphicsDropShadowEffect(this);
    m_effect->setXOffset(0);
    m_effect->setYOffset(18);
    m_effect->setBlurRadius(60);
    m_effect->setColor(QColor(0,0,0,255*0.3));

    this->setGraphicsEffect(m_effect);
}

void KiranClock::updateDrawRect()
{
    ///计算表盘绘制区域,更新m_drawRect
    QRect drawRect;

    int shortSiderLength = this->width()<this->height()?this->width():this->height();

    drawRect.setX( (this->width()-shortSiderLength)/2 );
    drawRect.setY( (this->height()-shortSiderLength)/2 );
    drawRect.setWidth(shortSiderLength);
    drawRect.setHeight(shortSiderLength);

    m_drawRect = drawRect;
}

void KiranClock::paintDial(QPainter &painter)
{
    if(!m_dialReader.isValid()){
        return;
    }

    painter.save();
    m_dialReader.render(&painter,m_drawRect);
    painter.restore();
}

void KiranClock::paintHour(QPainter &painter)
{
    if( !m_dialReader.isValid() || !m_hourReader.isValid() ){
        return;
    }

    painter.save();

    int hour = QDateTime::currentDateTime().time().hour();
    int minute = QDateTime::currentDateTime().time().minute();

    ///坐标原点移动到绘制的中心，用于之后的指针旋转
    QPoint drawRectCenter(m_drawRect.x()+m_drawRect.width()/2,
                          m_drawRect.y()+m_drawRect.height()/2);
    painter.translate(drawRectCenter);
    painter.rotate(hour*30+minute/60*30);
    ///计算出绘制区域和表盘默认大小缩放比例,算出秒针头长度、秒针绘制区域
    QRectF hourDrawRect;

    double scaleFactor = m_drawRect.width()/m_dialReader.defaultSize().width();
    double hour_w = m_hourReader.defaultSize().width() * scaleFactor;
    double hour_h = m_hourReader.defaultSize().height() * scaleFactor;
    double hour_head_l = hour_h * (1.0-HOUR_HAND_TAIR_RATIO);

    hourDrawRect.setX( -(hour_w/2) );
    hourDrawRect.setY( -hour_head_l );
    hourDrawRect.setWidth(hour_w);
    hourDrawRect.setHeight(hour_h);

    ///根据指针头长度计算出绘制区域
    m_hourReader.render(&painter,hourDrawRect);

    painter.restore();
}

void KiranClock::paintMinute(QPainter &painter)
{
    if( !m_dialReader.isValid() || !m_minuteReader.isValid() ){
        return;
    }

    painter.save();

    int minute = QDateTime::currentDateTime().time().minute();
    int second = QDateTime::currentDateTime().time().second();

    ///坐标原点移动到绘制的中心，用于之后的指针旋转
    QPoint drawRectCenter(m_drawRect.x()+m_drawRect.width()/2,
                          m_drawRect.y()+m_drawRect.height()/2);
    painter.translate(drawRectCenter);
    painter.rotate(minute*6.0+second/60.0*6.0);
    ///计算出绘制区域和表盘默认大小缩放比例,算出秒针头长度、秒针绘制区域
    QRectF minuteDrawRect;
    double scaleFactor = m_drawRect.width()/m_dialReader.defaultSize().width();
    double minute_w = m_minuteReader.defaultSize().width() * scaleFactor;
    double minute_h = m_minuteReader.defaultSize().height() * scaleFactor;
    double minute_head_l = minute_h * (1.0-MINUTE_HAND_TAIR_RATIO);

    minuteDrawRect.setX( -(minute_w/2) );
    minuteDrawRect.setY( -minute_head_l );
    minuteDrawRect.setWidth(minute_w);
    minuteDrawRect.setHeight(minute_h);

    ///根据指针头长度计算出绘制区域
    m_hourReader.render(&painter,minuteDrawRect);

    painter.restore();
}

void KiranClock::paintSecond(QPainter &painter)
{
    int sec = QDateTime::currentDateTime().time().second();
    int msec = QDateTime::currentDateTime().time().msec();

    if(!m_dialReader.isValid() || !m_secondReader.isValid() ){
        return;
    }

    painter.save();
    ///坐标原点移动到绘制的中心，用于之后的指针旋转
    QPoint drawRectCenter(m_drawRect.x()+m_drawRect.width()/2,
                          m_drawRect.y()+m_drawRect.height()/2);
    painter.translate(drawRectCenter);
    painter.rotate(sec*6.0+msec/1000.0*6.0);

    ///计算出绘制区域和表盘默认大小缩放比例,算出秒针头长度、秒针绘制区域
    QRectF secondDrawRect;
    double scaleFactor = m_drawRect.width()/m_dialReader.defaultSize().width();
    double second_w = m_secondReader.defaultSize().width() * scaleFactor;
    double second_h = m_secondReader.defaultSize().height() * scaleFactor;
    double second_head_l = second_h * (1.0-SECOND_HAND_TAIR_RATIO);

    secondDrawRect.setX( -(second_w/2) );
    secondDrawRect.setY( -second_head_l );
    secondDrawRect.setWidth(second_w);
    secondDrawRect.setHeight(second_h);

    ///根据指针头长度计算出绘制区域
    m_secondReader.render(&painter,secondDrawRect);

    painter.restore();
}

void KiranClock::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::HighQualityAntialiasing);

    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    paintDial(painter);
    paintHour(painter);
    paintMinute(painter);
    paintSecond(painter);


    QWidget::paintEvent(event);
}

void KiranClock::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateDrawRect();
}

void KiranClock::timerEvent(QTimerEvent *event)
{
    update();
}
