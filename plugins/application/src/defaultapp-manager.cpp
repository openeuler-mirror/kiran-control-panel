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

#include "defaultapp-manager.h"
#include <qt5-log-i.h>
#include <QFileInfo>
#include <QSettings>
#include <QTextStream>
#include "defaultapp-plugin.h"
#include "mimetype-cache.h"
#include "utils.h"

#define MIMEAPPS_LIST_DEFAULT_GROUP "Default Applications"

DefaultAppManager::DefaultAppManager()
{
}

DefaultAppManager::~DefaultAppManager()
{
}

XdgDesktopFile* DefaultAppManager::getMimeDefaultApp(EnumMimeType enumMimeType)
{
    AppMimeTypes mimeTypes = MimeAppsFileManager::defaultAppEnumToMimeTypes(enumMimeType);
    for (auto& iterMimetype : mimeTypes.mimeTypeForApps)
    {
        XdgMimeApps xdgMimeApps;
        XdgDesktopFile* defaultapp = xdgMimeApps.defaultApp(iterMimetype);

        if (defaultapp == nullptr)
        {
            continue;
        }
        return defaultapp;
    }
    return nullptr;
}

void DefaultAppManager::setDefaultApp(EnumMimeType enumMimeType, XdgDesktopFile* defaultDesktop)
{
    AppMimeTypes mimeTypes = MimeAppsFileManager::defaultAppEnumToMimeTypes(enumMimeType);

    XdgMimeApps xdgMimeApps;
    for (auto iter = (mimeTypes.mimeTypeForDefaultApps).begin(); iter != (mimeTypes.mimeTypeForDefaultApps).end(); iter++)
    {
        xdgMimeApps.setDefaultApp(*iter, *defaultDesktop);
    }
}
