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

#include "mimetype-cache.h"

MimeTypeCache::MimeTypeCache() : m_IsInitialized(false), m_xdgDesktopFile()
{
}

MimeTypeCache::~MimeTypeCache()
{
}

MimeTypeCache& MimeTypeCache::GetInstance()
{
    static MimeTypeCache instance;
    if (!instance.m_IsInitialized)
    {
        instance.loadMimeinfoCache();
        instance.m_IsInitialized = true;
    }

    return instance;
}

QVector<XdgDesktopFilePtr> MimeTypeCache::getMimeinfoCache(EnumMimeType enumMimeType)
{
    return GetInstance().m_xdgDesktopFile.value(enumMimeType);
}

void MimeTypeCache::loadMimeinfoCache()
{
    for (EnumMimeType enumMimeType = DA_TYPE_WEB_BROWSER;
         enumMimeType < DA_TYPE_LAST;
         enumMimeType = static_cast<EnumMimeType>(enumMimeType + 1))
    {
        AppMimeTypes mimeTypes = MimeAppsFileManager::defaultAppEnumToMimeTypes(enumMimeType);
        QVector<QString> uniqueFileName;
        QVector<XdgDesktopFilePtr> desktopInfo;

        for (auto& iterMimetype : mimeTypes.mimeTypeForApps)
        {
            XdgMimeApps xdgMimeApps;
            QList<XdgDesktopFile*> desktopFileList = xdgMimeApps.apps(iterMimetype);

            for (int index = 0; index < desktopFileList.size(); index++)
            {
                if (uniqueFileName.indexOf(desktopFileList[index]->fileName()) != -1)
                {
                    continue;
                }

                uniqueFileName.push_back(desktopFileList[index]->fileName());
                desktopInfo.push_back(QSharedPointer<XdgDesktopFile>(desktopFileList[index]));
            }
        }

        m_xdgDesktopFile.insert(enumMimeType, desktopInfo);
    }
}