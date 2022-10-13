/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#ifndef KIRAN_ACCOUNT_MANAGER_ANIMATION_PUSH_BUTTON_H
#define KIRAN_ACCOUNT_MANAGER_ANIMATION_PUSH_BUTTON_H

#include <QPushButton>
#include <QSvgRenderer>
#include <QTimeLine>

class AnimationButtonDelegate;

class AnimationPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit AnimationPushButton(QWidget *parent);
    ~AnimationPushButton() = default;

    void setBusy(bool busy);
    bool busy();

public Q_SLOTS:

private:
    void initTimeLine();

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    bool m_isBusy = false;
    QTimeLine m_timeLine;
    QSvgRenderer m_svgRender;
    int m_rotationAngle = 0;
};

#endif  //KIRAN_ACCOUNT_MANAGER_ANIMATION_PUSH_BUTTON_H