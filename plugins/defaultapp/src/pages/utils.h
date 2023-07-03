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

#ifndef UTILS_H
#define UTILS_H

#include <QVector>

enum EnumMimeType
{
    DA_TYPE_UNKNOW = 0,
    // 浏览器支持的MimeType类型
    DA_TYPE_WEB_BROWSER,
    // 邮件支持的MimeType类型
    DA_TYPE_EMAIL,
    // 文件编辑器支持的MimeType类型
    DA_TYPE_TEXT,
    // 媒体播放器支持的MimeType类型
    DA_TYPE_MEDIA,
    // 视频播放器支持的MimeType类型
    DA_TYPE_VIDEO,
    // 照片编辑器支持的MimeType类型
    DA_TYPE_IMAGE,
    // pdf文档编辑器支持的MimeType类型
    DA_TYPE_DOCUMENT,
    // word文档编辑器支持的MimeType类型
    DA_TYPE_WORD,
    // excel文档编辑器支持的MimeType类型
    DA_TYPE_SPREADSHEET
};

class MimeTypes
{
public:
    static QMap<EnumMimeType, QVector<QString>> enumToMimeTypes;
};

class UTILS
{
public:
    static QVector<QString> defaultAppEnumToMimeTypes(EnumMimeType enumMimeType);
    static EnumMimeType mimeTypesToDefaultAppEnum(QString mimeType);
    static QString getMimeAppsListFile();
    static void setMimeAppsList(QString& mimeAppsListFile, QString& mimeType, QString defaultDesktop);
    static void setMimeAppsListGroupDefault(QString& mimeAppsListFile, QString& mimeType, QString& defaultDesktop);
    static void createMimeAppsList(QMap<EnumMimeType, QString>& mimeAppsListInfo, QString& mimeAppsListFile);
};

#endif  // UTILS_H
