/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-mouse is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#ifndef MOUSE_PAGE_H
#define MOUSE_PAGE_H

#include <QComboBox>
#include <QListWidgetItem>
#include <QSharedPointer>
#include <QWidget>

class KSMMouseProxy;
class KiranSwitchButton;
class KiranListItem;
class KiranSwitchButton;
namespace Ui
{
class MousePage;
}

class MousePage : public QWidget
{
    Q_OBJECT

public:
    explicit MousePage(QWidget *parent = 0);
    ~MousePage();
    void initUI();
    virtual QSize sizeHint() const override;

public:
    void initComponent();

public slots:
    void onSliderValueChange();

private:
    Ui::MousePage *ui;
    QSharedPointer<KSMMouseProxy> m_mouseInterface;

    bool m_mouseLeftHand = false;
    bool m_mouseNaturalScroll = false;
    bool m_middleEmulationEnabled = false;
    double m_mouseMotionAcceleration = 0.0;

    //鼠标是否按下的标志位
    bool m_mousePressed = false;
    QTimer *m_timer = nullptr;
};

#endif  // MOUSE_PAGE_H
