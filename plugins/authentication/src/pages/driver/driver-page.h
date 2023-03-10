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
#include <QWidget>
#include <kiran-authentication-service/kas-authentication-i.h>

class AuthSettingContainer;
class KiranAuthDBusProxy;
class QComboBox;
class DriverPage : public QWidget
{
    Q_OBJECT
public:
    DriverPage(KiranAuthDBusProxy* proxy,QWidget* parent = nullptr);
    ~DriverPage();

private:
    void initUI();
    void refreshAllDriver();

private slots:
    void onComboxBoxCurrentIndexChanged(int index);
    void onDriverSwticherToggled(const QString& driverName,bool checked);

private:
    KiranAuthDBusProxy* m_proxy;
    QComboBox* m_combobox;
    AuthSettingContainer* m_container;
    KADAuthType m_authType;
};