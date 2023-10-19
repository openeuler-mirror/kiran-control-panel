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

#include "app-manager.h"
#include <qt5-log-i.h>
#include <QDebug>
#include <QMap>
#include "defaultapp-manager.h"
#include "mimetype-cache.h"

AppManager::AppManager()
{
    getAppsForMimeType();
}

AppManager::~AppManager()
{
}

void AppManager::getAppsForMimeType()
{
    for (EnumMimeType enumMimeType = DA_TYPE_WEB_BROWSER;
         enumMimeType < DA_TYPE_LAST;
         enumMimeType = static_cast<EnumMimeType>(enumMimeType + 1))
    {
        QVector<XdgDesktopFilePtr> mimeInfoCache = MimeTypeCache::getMimeinfoCache(enumMimeType);
        XdgDesktopFile* defaultApp = DefaultAppManager::getMimeDefaultApp(enumMimeType);

        if (mimeInfoCache.isEmpty())
        {
            continue;
        }
        if (defaultApp == nullptr)
        {
            m_appInfos.insert(enumMimeType, mimeInfoCache);
            continue;
        }

        sortApplication(enumMimeType, mimeInfoCache, defaultApp);
        m_appInfos.insert(enumMimeType, mimeInfoCache);

        delete defaultApp;
        defaultApp = nullptr;
    }
}

QVector<XdgDesktopFilePtr> AppManager::getApplications(EnumMimeType enumMimeType)
{
    if (!m_appInfos.contains(enumMimeType))
    {
        return QVector<XdgDesktopFilePtr>();
    }
    return m_appInfos.value(enumMimeType);
}

void AppManager::sortApplication(EnumMimeType& enumMimeType, QVector<XdgDesktopFilePtr>& mimeInfoCache, XdgDesktopFile* defaultApp)
{
    int index = -1;
    for (int i = 0; i < mimeInfoCache.size(); i++)
    {
        if (mimeInfoCache[i].data()->fileName() == defaultApp->fileName())
        {
            index = i;
            break;
        }
    }
    if (index == -1)
    {
        KLOG_DEBUG("mimeapps.list error, refresh it.");
        DefaultAppManager::setDefaultApp(enumMimeType, mimeInfoCache[0].data());
        return;
    }
    XdgDesktopFilePtr value = mimeInfoCache[index];
    mimeInfoCache.removeAt(index);
    mimeInfoCache.push_front(value);
}

void AppManager::slotTextChanged(EnumMimeType enumMimeType, XdgDesktopFile* desktopFile)
{
    DefaultAppManager::setDefaultApp(enumMimeType, desktopFile);
}