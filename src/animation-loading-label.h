/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-cpanel-network is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinos.com.cn>
 */

#ifndef KIRAN_CPANEL_NETWORK_ANIMATION_LOADING_LABEL_H
#define KIRAN_CPANEL_NETWORK_ANIMATION_LOADING_LABEL_H

#include <QLabel>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QWidget>

class AnimationLoadingLabel : public QLabel
{
    Q_OBJECT

    Q_PROPERTY(qreal dashOffset WRITE setDashOffset READ dashOffset)
    Q_PROPERTY(qreal dashLength WRITE setDashLength READ dashLength)
    Q_PROPERTY(int angle WRITE setAngle READ angle)
public:
    explicit AnimationLoadingLabel(QWidget *parent = nullptr);

    void init();
    void paintEvent(QPaintEvent *event) override;
    void setSize(int size);
    void setLoadingStatus(bool isLoading);

    inline void setDashOffset(qreal offset);
    inline qreal dashOffset() const;

    inline void setDashLength(qreal length);
    inline qreal dashLength() const;

    inline void setAngle(int angle);
    inline int angle() const;

private:
    qreal m_dashOffset;
    qreal m_dashLength;
    int m_angle;
    int m_size;

    bool m_isLoading;
    QParallelAnimationGroup *m_group;
    QPropertyAnimation *m_animation;
};

inline void AnimationLoadingLabel::setDashOffset(qreal offset)
{
    m_dashOffset = offset;
    this->update();
}

inline qreal AnimationLoadingLabel::dashOffset() const
{
    return m_dashOffset;
}

inline void AnimationLoadingLabel::setDashLength(qreal length)
{
    m_dashLength = length;
    this->update();
}

inline qreal AnimationLoadingLabel::dashLength() const
{
    return m_dashLength;
}

inline void AnimationLoadingLabel::setAngle(int angle)
{
    m_angle = angle;
    this->update();
}

inline int AnimationLoadingLabel::angle() const
{
    return m_angle;
}


#endif  //KIRAN_CPANEL_NETWORK_ANIMATION_LOADING_LABEL_H
