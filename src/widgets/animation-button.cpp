//
// Created by lxh on 2020/11/2.
//
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPainter>
#include <QSize>

#include "animation-button.h"
#include "animation-button-delegate.h"

AnimationButton::AnimationButton(QWidget *parent)
        : QAbstractButton(parent) ,
          m_animationDelegate(new AnimationButtonDelegate(this)){
    initAnimation();
}

AnimationButton::~AnimationButton() {

}

int AnimationButton::circleSize() {
    return m_circleSize;
}

int AnimationButton::circleWidth() {
    return m_circleWidth;
}

void AnimationButton::setCircleWidth(int lineWidth) {
    if( m_circleWidth == lineWidth ){
        return;
    }
    m_circleWidth = lineWidth;
    update();
}

void AnimationButton::setCircleSize(int size) {
    if( m_circleSize == size ){
        return;
    }
    m_circleSize = size;
    update();
}

#define BUTTON_PAINT_SPACING 5
void AnimationButton::paintEvent(QPaintEvent *e) {
    //TODO:绘制图标,文本,还有转圈图标
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);

    QIcon btnIcon = icon();
    QSize btnIconSize = iconSize();

    QSize circleSize(m_circleSize,m_circleSize);

    int textMaxWidth = width()-btnIconSize.width()-circleSize.width()-2*BUTTON_PAINT_SPACING;

    ///绘制图标
    QPixmap btnPixmap = btnIcon.pixmap(btnIconSize);
//    painter.drawPixmap();

    ///绘制文字
    QFont btnFont = font();
    QFontMetrics metrics(btnFont,this);
    QString showText = text();
    if( m_elideTextEnable ){

    }
//    QString metrics.elidedText()

    ///绘制旋转等待图标
}

void AnimationButton::setBusy(bool busy) {
    if(busy){
        m_animationGroup->start();
    }else{
        m_animationGroup->stop();
    }
}

bool AnimationButton::isBusy() {
    return m_animationGroup->state()==QAbstractAnimation::Running;
}

void AnimationButton::initAnimation() {
    m_animationGroup = new QParallelAnimationGroup(this);

    auto propertyAnimation = new QPropertyAnimation(this);
    m_animationGroup->addAnimation(propertyAnimation);
    propertyAnimation->setTargetObject(m_animationDelegate);
    propertyAnimation->setPropertyName("dashLength");
    propertyAnimation->setStartValue(0.1);
    propertyAnimation->setKeyValueAt(0.15, 0.2);
    propertyAnimation->setKeyValueAt(0.6, 20);
    propertyAnimation->setKeyValueAt(0.7, 20);
    propertyAnimation->setEndValue(20);
    propertyAnimation->setDuration(2050);

    propertyAnimation = new QPropertyAnimation(this);
    m_animationGroup->addAnimation(propertyAnimation);
    propertyAnimation->setTargetObject(m_animationDelegate);
    propertyAnimation->setPropertyName("dashOffset");
    propertyAnimation->setStartValue(0);
    propertyAnimation->setKeyValueAt(0.15, 0);
    propertyAnimation->setKeyValueAt(0.6, -7);
    propertyAnimation->setKeyValueAt(0.7, -7);
    propertyAnimation->setEndValue(-25);
    propertyAnimation->setDuration(2050);

    propertyAnimation = new QPropertyAnimation(this);
    m_animationGroup->addAnimation(propertyAnimation);
    propertyAnimation->setTargetObject(m_animationDelegate);
    propertyAnimation->setPropertyName("angle");
    propertyAnimation->setStartValue(0);
    propertyAnimation->setEndValue(719);
    propertyAnimation->setDuration(2050);
}

void AnimationButton:: setElideTextEnable(bool enable) {
    if( m_elideTextEnable==enable ){
        return;
    }
    m_elideTextEnable = enable;
    update();
}

bool AnimationButton::elideTextEnable() {
    return m_elideTextEnable;
}

QColor AnimationButton::circleColor() {
    return m_circleColor;
}

void AnimationButton::setCircleColor(QColor color) {

}
