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

#ifndef TOUCHPAD_PAGE_H
#define TOUCHPAD_PAGE_H

#include <QComboBox>
#include <QLabel>
#include <QWidget>

namespace Ui
{
class TouchPadPage;
}

class KiranSwitchButton;
class KSMTouchPadProxy;
class TouchPadPage : public QWidget
{
    Q_OBJECT

public:
    explicit TouchPadPage(QWidget *parent = 0);
    ~TouchPadPage();

public:
    void initUI();

    virtual QSize sizeHint() const override;

private:
    void addComboBoxItem();
    void setDisableWidget(bool);
    void initComponent();

public slots:
    void onSliderValueChange();
    void onDisabelTouchPadToggled(bool disabled);

private:
    Ui::TouchPadPage *ui;
    QSharedPointer<KSMTouchPadProxy> m_touchPadInterface;
    QList<QComboBox *> m_comboBoxList;
    QList<KiranSwitchButton *> m_checkBoxList;
    QList<QLabel *> m_labelList;

    bool m_disabelWhileTyping = true;
    bool m_touchPadLeftHand = false;
    bool m_touchPadNaturalScroll = false;
    bool m_tapToClick = true;
    bool m_touchPadEnabled = true;
    double m_touchPadMotionAcceleration = 0.0;
    int m_clickMethod = 0;
    int m_scrollMethod = 0;

    //鼠标是否按下的标志位
    bool m_mousePressed = false;
    QTimer *m_timer = nullptr;
};

#endif  // TOUCHPAD_PAGE_H
