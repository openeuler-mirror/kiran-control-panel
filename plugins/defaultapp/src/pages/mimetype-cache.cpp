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

QVector<XdgDesktopFile*> MimeTypeCache::getMimeinfoCache(EnumMimeType enumMimeType)
{
    return GetInstance().m_xdgDesktopFile.value(enumMimeType);
}

void MimeTypeCache::loadMimeinfoCache()
{
    for (EnumMimeType enumMimeType = DA_TYPE_WEB_BROWSER;
         enumMimeType <= DA_TYPE_SPREADSHEET;
         enumMimeType = static_cast<EnumMimeType>(enumMimeType + 1))
    {
        QVector<QString> mimeTypes = UTILS::defaultAppEnumToMimeTypes(enumMimeType);
        QVector<QString> uniqueFileName;
        QVector<XdgDesktopFile*> desktopInfo;

        for (auto& iterMimetype : mimeTypes)
        {
#ifdef VERSION_QTXDG331
            QList<XdgDesktopFile*> desktopFileList = XdgDesktopFileCache::getApps(iterMimetype);
#else
            XdgMimeApps xdgMimeApps;
            QList<XdgDesktopFile*> desktopFileList = xdgMimeApps.apps(iterMimetype);
#endif
            for (int index = 0; index < desktopFileList.size(); index++)
            {
                if (uniqueFileName.indexOf(desktopFileList[index]->fileName()) != -1)
                {
                    continue;
                }

                uniqueFileName.push_back(desktopFileList[index]->fileName());
                desktopInfo.push_back(desktopFileList[index]);
            }
        }

        m_xdgDesktopFile.insert(enumMimeType, desktopInfo);
    }
}