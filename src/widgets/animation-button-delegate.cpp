//
// Created by lxh on 2020/11/3.
//

#include "animation-button-delegate.h"

AnimationButtonDelegate::AnimationButtonDelegate(AnimationPushButton* btn)
    :QObject(btn),
    m_button(btn){

}

qreal AnimationButtonDelegate::dashOffset() {
    return  m_offset;
}

qreal AnimationButtonDelegate::dashLength() {
    return m_length;
}

int AnimationButtonDelegate::angle() {
    return m_angle;
}

void AnimationButtonDelegate::setDashOffset(qreal offset) {
    if( m_offset==offset ){
        return;
    }
    m_offset = offset;
    m_button->update(m_button->rect());
}

void AnimationButtonDelegate::setDashLength(qreal length) {
    if( m_length==length ){
        return;
    }
    m_length = length;
    m_button->update(m_button->rect());
}

void AnimationButtonDelegate::setAngle(int angle) {
    if( m_angle==angle ){
        return;
    }
    m_angle = angle;
    m_button->update(m_button->rect());
}
