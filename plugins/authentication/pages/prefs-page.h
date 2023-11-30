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

#include <kiran-authentication-service/kas-authentication-i.h>
#include <QWidget>
#include <QMap>

class QStackedWidget;
class QLabel;
class KiranAuthDBusProxy;
class AuthSettingItem;
class PrefsPage : public QWidget
{
    Q_OBJECT
public:
    PrefsPage(KiranAuthDBusProxy* proxy, QWidget* parent = nullptr);
    ~PrefsPage();

private:
    void initUI();
    QWidget* initAuthTypePage();
    QWidget* initAuthApplicationPage();

private:
    void refreshAuthTypeEnabled();
    void refreshAuthApplicationEanbled();

private slots:
    void updateCurrentAuthType(KADAuthType authType);
    void updateAuthTypeEnable(KADAuthType authType,bool checked);
    void updateAuthTypeEnableForApp(KADAuthApplication authApp,bool checked);
    void toPrefsHomePage();

private:
    KiranAuthDBusProxy* m_proxy;

    QMap<KADAuthType,AuthSettingItem*> m_authTypeMap;
    QMap<KADAuthApplication,AuthSettingItem*> m_authAppMap;

    QLabel* m_labelAuthApp;
    QStackedWidget* m_stackedWidget;
    KADAuthType m_currentAuthType = KAD_AUTH_TYPE_NONE;
};