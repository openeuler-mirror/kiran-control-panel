#include "defaultapp-frame.h"
#include <qt5-log-i.h>
#include <QDebug>
#include <QMap>
#include "defaultapp-manager.h"
#include "mimetype-cache.h"

DefaultAppFrame::DefaultAppFrame()
{
    getAppsForMimeType();
}

DefaultAppFrame::~DefaultAppFrame()
{
}

void DefaultAppFrame::getAppsForMimeType()
{
    for (EnumMimeType enumMimeType = DA_TYPE_WEB_BROWSER;
         enumMimeType <= DA_TYPE_SPREADSHEET;
         enumMimeType = static_cast<EnumMimeType>(enumMimeType + 1))
    {
        QVector<XdgDesktopFile*> mimeInfoCache = MimeTypeCache::getMimeinfoCache(enumMimeType);
        XdgDesktopFile* defaultApp = DefaultAppManager::getMimeDefaultApp(enumMimeType);

        if (mimeInfoCache.isEmpty())
        {
            continue;
        }
        if (defaultApp == nullptr)
        {
            appInfos.insert(enumMimeType, mimeInfoCache);
            continue;
        }

        sortApplication(enumMimeType, mimeInfoCache, defaultApp);
        appInfos.insert(enumMimeType, mimeInfoCache);
    }
}

QVector<XdgDesktopFile*> DefaultAppFrame::getApplications(EnumMimeType enumMimeType)
{
    if (!appInfos.contains(enumMimeType))
    {
        return QVector<XdgDesktopFile*>();
    }
    return appInfos.value(enumMimeType);
}

void DefaultAppFrame::sortApplication(EnumMimeType& enumMimeType, QVector<XdgDesktopFile*>& mimeInfoCache, XdgDesktopFile* defaultApp)
{
    int index = -1;
    for (int i = 0; i < mimeInfoCache.size(); i++)
    {
        if (mimeInfoCache[i]->fileName() == defaultApp->fileName())
        {
            index = i;
            break;
        }
    }
    if (index == -1)
    {
        KLOG_DEBUG("mimeapps.list error, refresh it.");
        DefaultAppManager::setDefaultApp(enumMimeType, mimeInfoCache[0]);
        return;
    }
    XdgDesktopFile* value = mimeInfoCache[index];
    mimeInfoCache.removeAt(index);
    mimeInfoCache.push_front(value);
}

void DefaultAppFrame::slotTextChanged(EnumMimeType enumMimeType, XdgDesktopFile* desktopFile)
{
    DefaultAppManager::setDefaultApp(enumMimeType, desktopFile);
}