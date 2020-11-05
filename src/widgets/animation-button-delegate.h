//
// Created by lxh on 2020/11/3.
//

#ifndef KIRAN_ACCOUNT_MANAGER_ANIMATION_BUTTON_DELEGATE_H
#define KIRAN_ACCOUNT_MANAGER_ANIMATION_BUTTON_DELEGATE_H

#include <QObject>

#include "animation-push-button.h"


class AnimationButtonDelegate : public QObject{
    Q_OBJECT
    Q_PROPERTY(qreal dashOffset WRITE setDashOffset READ dashOffset)
    Q_PROPERTY(qreal dashLength WRITE setDashLength READ dashLength)
    Q_PROPERTY(int angle WRITE setAngle READ angle)
public:
    AnimationButtonDelegate(AnimationPushButton* btn);
    ~AnimationButtonDelegate() = default;

    qreal dashOffset();
    qreal dashLength();
    int angle();

public Q_SLOTS:
    void setDashOffset(qreal offset);
    void setDashLength(qreal length);
    void setAngle(int angle);

private:
    AnimationPushButton *const m_button;
    qreal m_offset = 0.0;
    qreal m_length = 0.1;
    int m_angle = 0;
};

#endif //KIRAN_ACCOUNT_MANAGER_ANIMATION_BUTTON_DELEGATE_H
