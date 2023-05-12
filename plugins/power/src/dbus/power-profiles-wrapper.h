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

#include <QObject>
#include <QDBusMessage>

class PowerProfilesProxy;
class PowerProfilesWrapper : public QObject
{
    Q_OBJECT
public:
    PowerProfilesWrapper(QObject* parent = nullptr);
    ~PowerProfilesWrapper();

    bool isValid();

    QStringList supportedProfiles();
    QString activeProfile();
    void setActiveProfile(const QString& profileName);

private slots:
    void onActiveProfileChanged(const QString& profileName);

signals:
    void activeProfileChanged(const QString& profileName);

private:
    // 由英文的ProfileName装换成本地locale的翻译文本
    QString profileNameTransToLocale(const QString& profileName);
    // 由于locale的翻译文本，转换成英文的ProfileName
    QString localeTransToProfileName(const QString& locale);

private:
    PowerProfilesProxy* m_profileProxy = nullptr;
    bool m_isValid = false;
    QString m_activeProfile;
    QStringList m_profiles;
};