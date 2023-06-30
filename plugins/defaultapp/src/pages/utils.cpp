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

#include "utils.h"
#include <QMap>
#include <QSettings>
#include "libqtxdg.h"

#define MIMEAPPS_LIST_DEFAULT_GROUP "Default Applications"

QMap<EnumMimeType, QVector<QString>> MimeTypes::enumToMimeTypes = {
    {DA_TYPE_WEB_BROWSER, {"x-scheme-handler/http", "x-scheme-handler/https", "text/html", "x-scheme-handler/about"}},
    {DA_TYPE_EMAIL, {"x-scheme-handler/mailto", "application/x-extension-eml", "message/rfc822"}},
    {DA_TYPE_TEXT, {"text/plain"}},
    {DA_TYPE_MEDIA, {"audio/mpeg", "audio/x-mpegurl", "audio/x-scpls", "audio/x-vorbis+ogg", "audio/x-wav"}},
    {DA_TYPE_VIDEO, {"video/mp4", "video/mpeg", "video/mp2t", "video/msvideo", "video/quicktime", "video/webm", "video/x-avi", "video/x-flv", "video/x-matroska", "video/x-mpeg", "video/x-ogm+ogg"}},
    {DA_TYPE_IMAGE, {"image/bmp", "image/gif", "image/jpeg", "image/png", "image/tiff"}},
    {DA_TYPE_DOCUMENT, {"application/pdf"}},
    {DA_TYPE_WORD, {"application/vnd.oasis.opendocument.text", "application/rtf", "application/msword", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"}},
    {DA_TYPE_SPREADSHEET, {"application/vnd.oasis.opendocument.spreadsheet", "application/vnd.ms-excel", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"}}};

QVector<QString> UTILS::defaultAppEnumToMimeTypes(EnumMimeType enumMimeType)
{
    return MimeTypes::enumToMimeTypes.value(enumMimeType);
}

EnumMimeType UTILS::mimeTypesToDefaultAppEnum(QString mimeType)
{
    foreach (const EnumMimeType& key, MimeTypes::enumToMimeTypes.keys())
    {
        QVector<QString> value = MimeTypes::enumToMimeTypes.value(key);
        if (value.indexOf(mimeType) != -1)
        {
            return key;
        }
    }
    return DA_TYPE_UNKNOW;
}

QString UTILS::getMimeAppsListFile()
{
    return QString("%1/.config/mimeapps.list").arg(getenv("HOME"));
}

#ifdef VERSION_QTXDG331
void UTILS::setMimeAppsListGroupDefault(QString& mimeAppsListFile, QString& mimeType, QString& defaultDesktop)
{
    QSettings defaults(mimeAppsListFile, XdgDesktopFileCache::desktopFileSettingsFormat());
    defaults.beginGroup(QLatin1String(MIMEAPPS_LIST_DEFAULT_GROUP));
    defaults.setValue(mimeType, defaultDesktop);
    defaults.endGroup();

    defaults.sync();
}

void UTILS::setMimeAppsList(QString& mimeAppsListFile, QString& mimeType, QString defaultDesktop)
{
    setMimeAppsListGroupDefault(mimeAppsListFile, mimeType, defaultDesktop);
}

void UTILS::createMimeAppsList(QMap<EnumMimeType, QString>& mimeAppsListInfo, QString& mimeAppsListFile)
{
    QSettings defaults(mimeAppsListFile, XdgDesktopFileCache::desktopFileSettingsFormat());

    defaults.beginGroup(QLatin1String(MIMEAPPS_LIST_DEFAULT_GROUP));
    foreach (const EnumMimeType& key, mimeAppsListInfo.keys())
    {
        QVector<QString> mimeTypes = UTILS::defaultAppEnumToMimeTypes(key);
        for (auto& iterMimeType : mimeTypes)
        {
            defaults.setValue(iterMimeType, mimeAppsListInfo.value(key));
        }
    }
    defaults.endGroup();

    defaults.sync();
}

#endif
