/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinsec.com.cn>
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

    Q_PROPERTY(int angle WRITE setAngle READ angle)
public:
    explicit AnimationLoadingLabel(QWidget *parent = nullptr);

    void init();
    void paintEvent(QPaintEvent *event) override;
    void setRadiusSize(int size);
    void setLineWidth(int size);
    void setLoadingStatus(bool isLoading);
    inline void setAngle(int angle);
    inline int angle() const;

private:
    int m_angle;
    int m_radiusSize;
    int m_lineWidth;
    bool m_isLoading;
    QParallelAnimationGroup *m_group;
    QPropertyAnimation *m_animation;
};



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
