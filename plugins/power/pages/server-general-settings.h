/**
 * Copyright (c) 2020 ~ 2024 KylinSec Co., Ltd.
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

#include <QTimer>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class ServerGeneralSettings;
}
QT_END_NAMESPACE

class KiranSwitchButton;
class QGSettings;
class ServerGeneralSettings : public QWidget
{
    Q_OBJECT
public:
    explicit ServerGeneralSettings(QWidget* parent = nullptr);
    ~ServerGeneralSettings() override;
    QSize sizeHint() const override;

private:
    void init();
    void initSessionSettings();
    void initScreensaverSettings();
    void initUI();

private slots:
    void updateIdleTimeLabel(int min);

private:
    Ui::ServerGeneralSettings* ui;
    QTimer m_idleTimeSettingTimer;
    QGSettings* m_sessionSettings = nullptr;
    QGSettings* m_screensaverSettings = nullptr;
    KiranSwitchButton* m_switchIdlelockScreen = nullptr;
};