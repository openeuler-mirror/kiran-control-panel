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
#pragma once
#include <QLabel>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QWidget>
#include <QPixmap>

namespace Kiran
{
namespace Network
{
class ConnectionIndicator : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(int angle WRITE setAngle READ angle)
public:
    explicit ConnectionIndicator(QWidget *parent = nullptr);

    void setLoadingStatus(bool isLoading);
    void setActivated(bool isActivated);

    void setAngle(int angle);
    int angle() const;

private:
    void init();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_angle = 0;
    const int m_radiusSize = 8;
    const int m_lineWidth = 2;
    bool m_isLoading = false;
    bool m_isActivated = false;
    QParallelAnimationGroup *m_group = nullptr;
    QPropertyAnimation *m_animation = nullptr;
    QPixmap m_connectedPixmap;
    QPixmap m_hoverPixmap;
};
}  // namespace Network
}  // namespace Kiran