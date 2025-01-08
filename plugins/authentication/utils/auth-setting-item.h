/**
 * Copyright (c) 2020 ~ 2023 KylinSec Co., Ltd.
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
#pragma once

#include <kiran-frame/kiran-frame.h>
#include <QVariant>

class KiranLabel;
class QPushButton;
class KiranSwitchButton;
class AuthSettingItem : public KiranFrame
{
    Q_OBJECT
public:
    explicit AuthSettingItem(QWidget* parent = nullptr);
    virtual ~AuthSettingItem();

    void setUserData(QVariant userData);
    QVariant getUserData() { return m_userData; };

    void setText(const QString& text);
    QString getText();

    void setLeftButtonVisible(bool visible, const QString& iconPath = QString());
    void setRightButtonVisible(bool visible, const QString& iconPath = QString());

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
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QVariant m_userData;

    QPushButton* m_leftButton;
    KiranLabel* m_label;
    KiranSwitchButton* m_switcher;
    QPushButton* m_rightButton;

    bool m_clickable = false;
    bool m_clicked = false;
};
