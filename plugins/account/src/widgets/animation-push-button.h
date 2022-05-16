 /**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; If not, see <http: //www.gnu.org/licenses/>. 
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