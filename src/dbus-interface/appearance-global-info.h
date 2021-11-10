/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-appearance is licensed under Mulan PSL v2.
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

#ifndef APPEARANCEGLOBALINFO_H
#define APPEARANCEGLOBALINFO_H

#include <QObject>

class AppearanceBackEndProxy;
class AppearanceGlobalInfo : public QObject
{
    Q_OBJECT
public:
    AppearanceGlobalInfo(QObject *parent = nullptr);
    ~AppearanceGlobalInfo();

    static AppearanceGlobalInfo *instance();

    bool getAllThemes(int themeType, QString &themesJson);
    bool setTheme(int themeType, QString themeName);
    bool getTheme(int type, QString &theme);

    QString getDesktopBackground();
    bool setDesktopBackground(QString);

    QString getLockScreenBackground();
    bool setLockScreenBackground(QString);

    QStringList getFont(int type);
    bool setFont(int fontType, QStringList fontInfoList);

signals:
    void themeChanged(int type, const QString &theme_name);
    void fontChanged(int type, const QString &fontInfo);
    void desktopBackgroundChanged(const QString &value) const;
    void lockScreenBackgroundChanged(const QString &value) const;

private:
    AppearanceBackEndProxy *m_appearanceInterface;
};

#endif  // APPEARANCEGLOBALINFO_H
