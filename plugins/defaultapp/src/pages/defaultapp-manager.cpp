#include "defaultapp-manager.h"
#include <qt5-log-i.h>
#include <QFileInfo>
#include <QSettings>
#include <QTextStream>
#include "defaultapp-plugin.h"
#include "mimetype-cache.h"
#include "utils.h"

DefaultAppManager::DefaultAppManager()
{
}

DefaultAppManager::~DefaultAppManager()
{
}

XdgDesktopFile* DefaultAppManager::getMimeDefaultApp(EnumMimeType enumMimeType)
{
    QVector<QString> mimeTypes = UTILS::defaultAppEnumToMimeTypes(enumMimeType);
    for (auto& iterMimetype : mimeTypes)
    {
#ifdef VERSION_QTXDG331
        XdgDesktopFile* defaultapp = XdgDesktopFileCache::getDefaultApp(iterMimetype);

#else
        XdgMimeApps xdgMimeApps;
        XdgDesktopFile* defaultapp = xdgMimeApps.defaultApp(iterMimetype);

#endif
        if (defaultapp == nullptr)
        {
            continue;
        }
        return defaultapp;
    }
    return nullptr;
}

#ifdef VERSION_QTXDG331

void DefaultAppManager::setDefaultApp(EnumMimeType enumMimeType, XdgDesktopFile* defaultDesktop)
{
    QVector<QString> mimeTypes = UTILS::defaultAppEnumToMimeTypes(enumMimeType);

    QString mimeAppsListFile = UTILS::getMimeAppsListFile();
    QFileInfo fileInfo(mimeAppsListFile);
    if (!fileInfo.isFile())
    {
        QMap<EnumMimeType, QString> mimeAppsListInfo;
        for (EnumMimeType enumMimeType = DA_TYPE_WEB_BROWSER;
             enumMimeType <= DA_TYPE_SPREADSHEET;
             enumMimeType = static_cast<EnumMimeType>(enumMimeType + 1))
        {
            QVector<XdgDesktopFile*> mimeInfoCache = MimeTypeCache::getMimeinfoCache(enumMimeType);
            if (mimeInfoCache.isEmpty())
                continue;
            mimeAppsListInfo.insert(enumMimeType, QFileInfo(mimeInfoCache[0]->fileName()).fileName());
        }

        UTILS::createMimeAppsList(mimeAppsListInfo, mimeAppsListFile);
        return;
    }

    for (auto iter = mimeTypes.begin(); iter != mimeTypes.end(); iter++)
    {
        UTILS::setMimeAppsList(mimeAppsListFile, *iter, QFileInfo(defaultDesktop->fileName()).fileName());
    }
}

#else

void DefaultAppManager::setDefaultApp(EnumMimeType enumMimeType, XdgDesktopFile* defaultDesktop)
{
    QVector<QString> mimeTypes = UTILS::defaultAppEnumToMimeTypes(enumMimeType);

    XdgMimeApps xdgMimeApps;
    for (auto iter = mimeTypes.begin(); iter != mimeTypes.end(); iter++)
    {
        xdgMimeApps.setDefaultApp(*iter, *defaultDesktop);
    }
}

#endif