/**
 * Copyright (c) 2022 ~ 2023 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
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


#ifndef __AUTOSTART_PAGE_H__
#define __AUTOSTART_PAGE_H__

#include <QWidget>
#include <QMap>

enum AutoStartAppFlags
{
    AUTO_START_APP_SUCCESS,
    AUTO_START_APP_EXISTS,
    AUTO_START_APP_NOPERMIT,
    AUTO_START_APP_NOSUPPORT
};

class QPushButton;
class AutostartApp;
class SettingsContainer;
class KiranSettingItem;
class AutostartPage : public QWidget
{
    Q_OBJECT

public:
    explicit AutostartPage(QWidget* parent = nullptr);
    virtual ~AutostartPage();

private:
    void initAutoStartAppUI();
    void initAddBtn();
    void initAutoStartApps();
    bool initAutoStartAppsConfig();

    KiranSettingItem* createAutoStartAppItem(const QString &desktopName, AutostartApp &autoStartApp);
    AutoStartAppFlags addAutoStartApp(const QString &path, const QString &desktopName);

private slots:
    void changeAutoStartAppStatus(const QString &desktopName, bool checked);
    void deleteAutoStartApp(KiranSettingItem* autoStartAppItem, const QString &desktopName);
    void selectDesktopForAutoStart();

private:
    SettingsContainer* autoStartAppContainer = nullptr;
    QPushButton* m_autoStartButton = nullptr;

    QMap<QString, AutostartApp> m_autoStartApps;

    QString m_localConfigDir;
};

#endif