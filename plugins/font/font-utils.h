/**
 * Copyright (c) 2020 ~ 2026 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */
#ifndef FONT_UTILS_H
#define FONT_UTILS_H

#include <QSet>
#include <QString>
#include <QStringList>

class FontUtils
{
public:
    static QString systemInstallDir();
    static QString userFontsDir();
    static QString userRejectConfPath();
    // 系统级禁用字体配置文件，目前禁用字体属于用户行为，仅保留该函数用于后续扩展。
    static QString systemRejectConfPath();

    static bool isFontFile(const QString &path);
    static bool isSystemFontPath(const QString &path);
    static bool isAllowedSystemFontPath(const QString &path);

    // 禁用配置文件相关操作
    static QSet<QString> readRejectList(const QString &confPath);
    static bool writeRejectList(const QString &confPath, const QSet<QString> &files, QString *error);
    static bool addRejectFiles(const QString &confPath, const QStringList &files, QString *error);
    static bool removeRejectFiles(const QString &confPath, const QStringList &files, QString *error);

    static bool copyFontFiles(const QStringList &srcFiles, const QString &destDir, QString *error);
    static bool removeFontFiles(const QStringList &files, QString *error);
    // 对指定目录（可为空表示全局）执行 fc-cache。
    static bool runFcCache(const QString &dir, QString *error);

private:
    static QStringList systemFontDirPrefixes();
};

#endif  // FONT_UTILS_H
