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
#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include "font-face.h"
#include "font-info.h"

#include <QList>
#include <QSet>
#include <QString>
#include <QStringList>

// 提供字体列表缓存与安装/删除/启用/禁用功能。
class FontManager
{
public:
    //获取表格展示用字体列表
    static QList<FontInfo> listFonts();
    //安装字体文件；system 为 true 时安装为系统字体，否则为用户字体。
    static bool addFonts(const QStringList &files, bool system, QString *error);
    //删除选中表格行对应的字体文件。
    static bool removeFonts(const QList<FontInfo> &fonts, QString *error);
    //启用或禁用选中表格行对应的字体（通过 reject 配置）。
    static bool setFontsEnabled(const QList<FontInfo> &fonts, bool enabled, QString *error);

private:
    // 获取禁用字体文件。
    static QSet<QString> allRejectedFiles();
    //确保 fontconfig 已初始化。
    static bool ensureFontconfig();
    //失效字体列表缓存。
    static void invalidateFontCache();
    // 刷新字体列表。
    static void updateFontList();
    // 重新初始化 fontconfig。
    static void reinitializeFontconfig();
    // 解析字体文件中的 face 信息。
    static FontFace faceFromPattern(void *pattern, const QSet<QString> &rejected);
    // 读取文件中的全部 face，追加到 out（seen 用于去重）。
    static void appendFacesFromFile(const QString &file, bool enabled, QList<FontFace> *out, QSet<QString> *seen);
    // 查询字体文件包含的全部 face，失败时写入 error。
    static bool queryFontFaces(const QString &file, QList<FontFace> *faces, QString *error);
    // 将 face 列表聚合成表格行（同文件多样式合并，同样式签名多文件合并）。
    static QList<FontInfo> toTableRows(const QList<FontFace> &faces);
    // 获取已安装的字体家族和样式值集合，用于安装前去重。
    static QSet<QString> installedFamilyStyles();
    // 获取待操作的字体文件。
    static void resolveTargetFiles(const QList<FontInfo> &selected, QStringList *systemFiles, QStringList *userFiles);

    static QList<FontFace> s_faces;
    static bool s_cacheInvalid;
};

#endif  // FONT_MANAGER_H
