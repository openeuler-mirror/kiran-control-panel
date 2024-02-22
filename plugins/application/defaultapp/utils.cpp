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

namespace MimeTypes
{
QMap<EnumMimeType, AppMimeTypes> enumToMimeTypes = {
    {DA_TYPE_WEB_BROWSER, {{"x-scheme-handler/http"}, {"x-scheme-handler/http", "x-scheme-handler/https", "text/html", "x-scheme-handler/about"}}},
    {DA_TYPE_EMAIL, {{"x-scheme-handler/mailto"}, {"x-scheme-handler/mailto", "application/x-extension-eml", "message/rfc822"}}},
    {DA_TYPE_TEXT, {{"text/plain"}, {"text/plain"}}},
    {DA_TYPE_MEDIA, {{"audio/x-vorbis+ogg"}, {"audio/mpeg", "audio/x-mpegurl", "audio/x-scpls", "audio/x-vorbis+ogg", "audio/x-wav"}}},
    {DA_TYPE_VIDEO, {
                        {"video/x-ogm+ogg"}, 
                        {"video/mp4", 
                        "video/mpeg", 
                        "video/mp2t", 
                        "video/msvideo", 
                        "video/quicktime", 
                        "video/webm", 
                        "video/x-avi", 
                        "video/x-flv", 
                        "video/x-matroska", 
                        "video/x-mpeg", 
                        "video/x-ogm+ogg"}
                    }},
    {DA_TYPE_IMAGE, {{"image/png"}, {"image/bmp", "image/gif", "image/jpeg", "image/png", "image/tiff"}}}
};
}

AppMimeTypes MimeAppsFileManager::defaultAppEnumToMimeTypes(EnumMimeType enumMimeType)
{
    return MimeTypes::enumToMimeTypes.value(enumMimeType);
}

QString MimeAppsFileManager::getMimeAppsListFile()
{
    return QString("%1/.config/mimeapps.list").arg(getenv("HOME"));
}
