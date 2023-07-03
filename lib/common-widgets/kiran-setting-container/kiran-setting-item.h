/**
 * Copyright (c) 2023 ~ 2024 KylinSec Co., Ltd.
 * ks-sc is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yinhongchang <yinhongchang@kylinsec.com.cn>
 */

#pragma once

#include <kiran-frame/kiran-frame.h>
#include <QIcon>
#include <QVariant>

class KiranLabel;
class QPushButton;
class KiranSwitchButton;
class KiranSettingItem : public KiranFrame
{
    Q_OBJECT
public:
    explicit KiranSettingItem(QWidget* parent = nullptr);
    virtual ~KiranSettingItem();

    void setUserData(QVariant userData);
    QVariant getUserData() { return m_userData; };

    void setText(const QString& text);
    QString getText();

    void setLeftButtonVisible(bool visible, const QIcon& icon, bool leftIconChanged = true);
    void setRightButtonVisible(bool visible, const QIcon& icon, bool rightIconChanged = true);

    void setSwitcherVisible(bool visible);
    void setSwitcherChecked(bool checked);

    void setClickable(bool clickable);
    bool getClickable();

signals:
    void leftButtonClicked(const QVariant& userData);
    void rightButtonClicked(const QVariant& userData);
    void switchButtonToggled(const QVariant& userData, bool checked);
    void clicked();

private:
    void initUI();
    Q_INVOKABLE void updateIcon();
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QVariant m_userData;

    QIcon m_leftIcon;
    QIcon m_rightIcon;

    QPushButton* m_leftButton;
    KiranLabel* m_label;
    KiranSwitchButton* m_switcher;
    QPushButton* m_rightButton;

    bool m_clickable = false;
    bool m_clicked = false;

    bool m_leftIconChanged;
    bool m_rightIconChanged;
};
