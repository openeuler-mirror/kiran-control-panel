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

#include "appearance-global-info.h"
#include <kiran-log/qt5-log-i.h>
#include <QMutex>
#include <QScopedPointer>
#include "appearance_backEnd_proxy.h"
#include "kiran-session-daemon/appearance-i.h"

using namespace std;
AppearanceGlobalInfo::AppearanceGlobalInfo(QObject *parent)
    : QObject(parent),
      m_appearanceInterface(nullptr)
{
    m_appearanceInterface = new AppearanceBackEndProxy(APPEARANCE_DBUS_NAME,
                                                       APPEARANCE_OBJECT_PATH,
                                                       QDBusConnection::sessionBus());
    connect(m_appearanceInterface, &AppearanceBackEndProxy::ThemeChanged,
            [this](int type, const QString &themeName) {
                emit themeChanged(type, themeName);
            });
    connect(m_appearanceInterface, &AppearanceBackEndProxy::desktop_backgroundChanged,
            [this](const QString &value) {
                emit desktopBackgroundChanged(value);
            });
    connect(m_appearanceInterface, &AppearanceBackEndProxy::lock_screen_backgroundChanged,
            [this](const QString &value) {
                emit lockScreenBackgroundChanged(value);
            });
}

AppearanceGlobalInfo::~AppearanceGlobalInfo()
{
}

AppearanceGlobalInfo *AppearanceGlobalInfo::instance()
{
    static QMutex mutex;
    static QScopedPointer<AppearanceGlobalInfo> pInst;

    if (Q_UNLIKELY(!pInst))
    {
        QMutexLocker locker(&mutex);
        if (pInst.isNull())
        {
            pInst.reset(new AppearanceGlobalInfo);
        }
    }

    return pInst.data();
}

bool AppearanceGlobalInfo::getAllThemes(int themeType, QString &themesJson)
{
    QDBusPendingReply<QString> reply = m_appearanceInterface->GetThemes(themeType);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        KLOG_DEBUG() << "Call GetThemes method failed : Theme type: " << themeType
                     << " Error: " << reply.error().message();
        return false;
    }
    else if (reply.count() < 1)
    {
        KLOG_DEBUG() << "Don't get correct reply!";
        return false;
    }
    else
    {
        themesJson = reply.argumentAt(0).toString();
        KLOG_INFO() << "theme type is : " << themeType
                    << " theme is :" << themesJson;
        return true;
    }
}

bool AppearanceGlobalInfo::setTheme(int themeType, QString themeName)
{
    QDBusPendingReply<> reply = m_appearanceInterface->SetTheme(themeType, themeName);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        KLOG_DEBUG() << "Call SetTheme method failed :"
                     << " Error: " << reply.error().message();
        return false;
    }
    else
        return true;
}

bool AppearanceGlobalInfo::getTheme(int themeType, QString &theme)
{
    QDBusPendingReply<QString> reply = m_appearanceInterface->GetTheme(themeType);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        KLOG_DEBUG() << "Call GetTheme method failed : Theme type: " << themeType
                     << " Error: " << reply.error().message();
        return false;
    }
    else if (reply.count() < 1)
    {
        KLOG_DEBUG() << "Don't get correct reply!";
        return false;
    }
    else
    {
        theme = reply.argumentAt(0).toString();
    }
    return true;
}

QString AppearanceGlobalInfo::getDesktopBackground()
{
    return m_appearanceInterface->desktop_background();
}

bool AppearanceGlobalInfo::setDesktopBackground(QString path)
{
    QDBusPendingReply<> reply = m_appearanceInterface->SetDesktopBackground(path);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        KLOG_DEBUG() << "Call set desktop background failed :"
                     << " Error: " << reply.error().message();
        return false;
    }
    else
        return true;
}

QString AppearanceGlobalInfo::getLockScreenBackground()
{
    return m_appearanceInterface->lock_screen_background();
}

bool AppearanceGlobalInfo::setLockScreenBackground(QString path)
{
    QDBusPendingReply<> reply = m_appearanceInterface->SetLockScreenBackground(path);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        KLOG_DEBUG() << "Call set lock screen background failed :"
                     << " Error: " << reply.error().message();
        return false;
    }
    else
        return true;
}

QStringList AppearanceGlobalInfo::getFont(int type)
{
    QString fontInfo;
    QStringList fontInfoList;
    QStringList font;
    QString fontName;
    QString fontSize;

    QDBusPendingReply<QString> reply = m_appearanceInterface->GetFont(type);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        KLOG_DEBUG() << "Call GetFont method failed : Font type: " << type
                     << " Error: " << reply.error().message();
    }

    else if (reply.count() < 1)
    {
        KLOG_DEBUG() << "Don't get correct reply!";
    }
    else
    {
        fontInfo = reply.argumentAt(0).toString();
        KLOG_INFO() << "Font type is: " << type
                    << " Font info is:" << fontInfo;

        fontInfoList = fontInfo.split(" ", QString::SkipEmptyParts);
        if (!fontInfoList.isEmpty())
        {
            fontSize = fontInfoList.takeLast();
            fontName = fontInfoList.join(" ");
            font << fontName << fontSize;
        }
    }
    return font;
}

bool AppearanceGlobalInfo::setFont(int fontType, QStringList fontInfoList)
{
    QString fontInfo;
    fontInfo = fontInfoList.join(" ");
    KLOG_INFO() << "setFont : fontInfo = " << fontInfo;

    QDBusPendingReply<> reply = m_appearanceInterface->SetFont(fontType, fontInfo);
    reply.waitForFinished();
    if (reply.isError() || !reply.isValid())
    {
        KLOG_DEBUG() << "Call SetFont method failed : Font type: " << fontType
                     << " Error: " << reply.error().message();
        return false;
    }
    return true;
}
