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
#include "font-manager.h"
#include "font-utils.h"
#include "logging-category.h"
#include "privileged-helper.h"

#include <fontconfig/fontconfig.h>

// fontconfig 2.18.1 起 FreeType 相关接口（含 FcFreeTypeQueryAll）只在 fcfreetype.h 中声明，
// 2.13 到 2.17 仍在 fontconfig.h 中。这里按 2.18 版本线（FC_VERSION >= 21800）统一处理：
// 2.18.0 尚未拆分，多包含一次该头无副作用；低版本则不引入 freetype 头依赖。
#if FC_VERSION >= 21800
#include <fontconfig/fcfreetype.h>
#endif

#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QObject>
#include <QSet>
#include <QStringList>
#include <algorithm>

// 存储所有字体文件中的字体信息
QList<FontFace> FontManager::s_faces;
// 缓存是否无效
bool FontManager::s_cacheInvalid = true;

namespace
{
QString fcString(FcPattern *pattern, const char *object)
{
    FcChar8 *value = nullptr;
    if (FcPatternGetString(pattern, object, 0, &value) != FcResultMatch || !value)
        return QString();
    return QString::fromUtf8(reinterpret_cast<const char *>(value));
}

int fcInt(FcPattern *pattern, const char *object, int fallback)
{
    int value = fallback;
    if (FcPatternGetInteger(pattern, object, 0, &value) != FcResultMatch)
        return fallback;
    return value;
}

quint32 createStyleValue(int weight, int width, int slant)
{
    return (static_cast<quint32>(weight & 0xFF) << 16) | (static_cast<quint32>(width & 0xFF) << 8) | static_cast<quint32>(slant & 0xFF);
}

QString styleNameFromAttributes(int weight, int slant)
{
    QStringList parts;
    if (weight >= FC_WEIGHT_HEAVY)
        parts << QStringLiteral("Heavy");
    else if (weight >= FC_WEIGHT_EXTRABOLD)
        parts << QStringLiteral("Extra Bold");
    else if (weight >= FC_WEIGHT_BOLD)
        parts << QStringLiteral("Bold");
    else if (weight >= FC_WEIGHT_DEMIBOLD)
        parts << QStringLiteral("Demi Bold");
    else if (weight >= FC_WEIGHT_MEDIUM && weight < FC_WEIGHT_DEMIBOLD)
        parts << QStringLiteral("Medium");
    else if (weight <= FC_WEIGHT_THIN)
        parts << QStringLiteral("Thin");
    else if (weight <= FC_WEIGHT_EXTRALIGHT)
        parts << QStringLiteral("Extra Light");
    else if (weight <= FC_WEIGHT_LIGHT)
        parts << QStringLiteral("Light");

    if (slant >= FC_SLANT_OBLIQUE)
        parts << QStringLiteral("Oblique");
    else if (slant >= FC_SLANT_ITALIC)
        parts << QStringLiteral("Italic");

    if (parts.isEmpty())
        return QStringLiteral("Regular");
    return parts.join(QLatin1Char(' '));
}

}  // namespace

QSet<QString> FontManager::allRejectedFiles()
{
    QSet<QString> rejected = FontUtils::readRejectList(FontUtils::userRejectConfPath());
    rejected.unite(FontUtils::readRejectList(FontUtils::systemRejectConfPath()));
    return rejected;
}

bool FontManager::ensureFontconfig()
{
    static bool initialized = false;
    if (!initialized)
    {
        if (!FcInit())
        {
            KLOG_ERROR(qLcFont) << "FcInit failed";
            return false;
        }
        initialized = true;
    }
    return true;
}

void FontManager::invalidateFontCache()
{
    s_faces.clear();
    s_cacheInvalid = true;
}

FontFace FontManager::faceFromPattern(void *patternPtr, const QSet<QString> &rejected)
{
    FcPattern *pattern = static_cast<FcPattern *>(patternPtr);
    FontFace face;

    face.family = fcString(pattern, FC_FAMILY);
    face.style = fcString(pattern, FC_STYLE);
    face.file = fcString(pattern, FC_FILE);
    face.faceIndex = fcInt(pattern, FC_INDEX, 0);

    const int weight = fcInt(pattern, FC_WEIGHT, FC_WEIGHT_REGULAR);
    const int width = fcInt(pattern, FC_WIDTH, FC_WIDTH_NORMAL);
    const int slant = fcInt(pattern, FC_SLANT, FC_SLANT_ROMAN);
    face.styleValue = createStyleValue(weight, width, slant);

    if (face.style.isEmpty())
        face.style = styleNameFromAttributes(weight, slant);

    face.isSystem = FontUtils::isSystemFontPath(face.file);
    face.enabled = !rejected.contains(face.file);
    return face;
}

void FontManager::appendFacesFromFile(const QString &file, bool enabled, QList<FontFace> *out, QSet<QString> *seen)
{
    FcFontSet *set = FcFontSetCreate();
    if (!set)
        return;

    int faceCount = 0;
    const QByteArray path = QFile::encodeName(file);
    const unsigned int added = FcFreeTypeQueryAll(reinterpret_cast<const FcChar8 *>(path.constData()),
                                                  static_cast<unsigned int>(-1),
                                                  nullptr,
                                                  &faceCount,
                                                  set);
    Q_UNUSED(faceCount);
    if (added == 0)
    {
        FcFontSetDestroy(set);
        return;
    }

    for (int i = 0; i < set->nfont; ++i)
    {
        FontFace face = faceFromPattern(set->fonts[i], QSet<QString>());
        face.enabled = enabled;
        face.isSystem = FontUtils::isSystemFontPath(face.file.isEmpty() ? file : face.file);
        if (face.file.isEmpty())
            face.file = file;
        if (face.family.isEmpty())
            continue;
        const QString key = face.faceKey();
        if (seen->contains(key))
            continue;
        seen->insert(key);
        out->append(face);
    }

    FcFontSetDestroy(set);
}

bool FontManager::queryFontFaces(const QString &file, QList<FontFace> *faces, QString *error)
{
    if (!faces)
        return false;

    faces->clear();

    FcFontSet *set = FcFontSetCreate();
    if (!set)
    {
        if (error)
            *error = QObject::tr("Not a valid font file: %1").arg(file);
        return false;
    }

    int faceCount = 0;
    const QByteArray path = QFile::encodeName(file);
    const unsigned int added = FcFreeTypeQueryAll(reinterpret_cast<const FcChar8 *>(path.constData()),
                                                  static_cast<unsigned int>(-1),
                                                  nullptr,
                                                  &faceCount,
                                                  set);
    Q_UNUSED(faceCount);
    if (added == 0)
    {
        FcFontSetDestroy(set);
        if (error)
            *error = QObject::tr("Not a valid font file: %1").arg(file);
        return false;
    }

    for (int i = 0; i < set->nfont; ++i)
    {
        FontFace face = faceFromPattern(set->fonts[i], QSet<QString>());
        face.file = file;
        face.isSystem = FontUtils::isSystemFontPath(file);
        if (face.family.isEmpty())
            continue;
        faces->append(face);
    }
    FcFontSetDestroy(set);

    if (faces->isEmpty())
    {
        if (error)
            *error = QObject::tr("Not a valid font file: %1").arg(file);
        return false;
    }
    return true;
}

// 将 face 级缓存转换为表格行（FontInfo）。
//
// 聚合目标：
//   1. 同一文件提供多个 style（如 TTC）→ 合并为一行，style 列显示 "Regular, Bold, ..."
//   2. 多个文件提供完全相同的 style 集合（如 C059-Bold.otf 与 C059-Bold.t1）→ 合并为一行，绑定全部文件
//
// 注意：合并依据是 styleValue（weight<<16|width<<8|slant），不是样式名字符串。
//       若 .otf/.t1 的 weight 不同（如 80 vs 100），即使样式名都是 Italic 也不会合并。
QList<FontInfo> FontManager::toTableRows(const QList<FontFace> &faces)
{
    using StyleMap = QMap<quint32, QString>;  // styleValue -> 显示名；QMap 按 key 有序

    // 第一步：按文件归集该文件上的全部 style（TTC 等多 face 先成一体）
    struct FileBundle
    {
        QString family;
        QString file;
        bool isSystem = true;
        bool enabled = true;
        StyleMap styles;
    };

    QMap<QString, FileBundle> byFile;  // key: family|isSystem|file
    Q_FOREACH (const FontFace &face, faces)
    {
        if (face.family.isEmpty() || face.file.isEmpty())
            continue;

        FileBundle &bundle = byFile[QStringLiteral("%1|%2|%3")
                                        .arg(face.family.toLower())
                                        .arg(face.isSystem ? 1 : 0)
                                        .arg(face.file)];
        if (bundle.family.isEmpty())
        {
            bundle.family = face.family;
            bundle.file = face.file;
            bundle.isSystem = face.isSystem;
            bundle.enabled = face.enabled;
        }
        else
        {
            bundle.enabled = bundle.enabled && face.enabled;
        }
        if (!bundle.styles.contains(face.styleValue))
            bundle.styles.insert(face.styleValue, face.style);
    }

    // 由有序 StyleMap 生成签名与展示字符串
    auto styleSigOf = [](const StyleMap &styles)
    {
        QStringList parts;
        for (auto it = styles.constBegin(); it != styles.constEnd(); ++it)
            parts << QString::number(it.key());
        return parts.join(QLatin1Char(','));
    };
    auto styleTextOf = [](const StyleMap &styles)
    {
        QStringList names;
        for (auto it = styles.constBegin(); it != styles.constEnd(); ++it)
            names << it.value();
        return names.join(QStringLiteral(", "));
    };

    // 第二步：按样式签名合并文件，直接得到 FontInfo
    QMap<QString, FontInfo> rows;  // key: family|isSystem|styleSig
    for (auto it = byFile.constBegin(); it != byFile.constEnd(); ++it)
    {
        const FileBundle &bundle = it.value();
        const QString styleSig = styleSigOf(bundle.styles);
        FontInfo &info = rows[QStringLiteral("%1|%2|%3")
                                  .arg(bundle.family.toLower())
                                  .arg(bundle.isSystem ? 1 : 0)
                                  .arg(styleSig)];
        if (info.family.isEmpty())
        {
            info.family = bundle.family;
            info.isSystem = bundle.isSystem;
            info.enabled = bundle.enabled;
            info.styleSig = styleSig;
            info.style = styleTextOf(bundle.styles);
        }
        else
        {
            info.enabled = info.enabled && bundle.enabled;
        }
        info.files << bundle.file;
    }

    QList<FontInfo> result = rows.values();
    for (FontInfo &info : result)
        std::sort(info.files.begin(), info.files.end());

    std::sort(result.begin(), result.end(), [](const FontInfo &a, const FontInfo &b)
              {
                  const int cmp = a.family.compare(b.family, Qt::CaseInsensitive);
                  if (cmp != 0)
                      return cmp < 0;
                  if (a.isSystem != b.isSystem)
                      return a.isSystem;
                  return a.style.compare(b.style, Qt::CaseInsensitive) < 0;
              });
    return result;
}

void FontManager::updateFontList()
{
    if (!ensureFontconfig())
        return;

    // 按需刷新
    const bool fcModified = !FcConfigUptoDate(nullptr);
    if (!fcModified && !s_cacheInvalid && !s_faces.isEmpty())
        return;

    if (fcModified)
    {
        if (!FcInitReinitialize())
            KLOG_WARNING(qLcFont) << "FcInitReinitialize failed, continue with current config";
        else
            KLOG_INFO(qLcFont) << "fontconfig reinitialized because config is out of date";
    }

    QList<FontFace> faces;
    const QSet<QString> rejected = allRejectedFiles();
    QSet<QString> seen;

    FcPattern *pattern = FcPatternCreate();
    FcObjectSet *objectSet = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_FILE, FC_INDEX,
                                              FC_WEIGHT, FC_WIDTH, FC_SLANT,
                                              nullptr);
    FcFontSet *fontSet = FcFontList(nullptr, pattern, objectSet);
    FcPatternDestroy(pattern);
    FcObjectSetDestroy(objectSet);

    if (fontSet)
    {
        for (int i = 0; i < fontSet->nfont; ++i)
        {
            FontFace face = faceFromPattern(fontSet->fonts[i], rejected);
            if (face.family.isEmpty() || face.file.isEmpty())
                continue;
            const QString key = face.faceKey();
            if (seen.contains(key))
                continue;
            seen.insert(key);
            faces.append(face);
        }
        FcFontSetDestroy(fontSet);
    }
    // 将禁用的字体文件添加到字体列表中，用于显示在表格中
    Q_FOREACH (const QString &file, rejected)
    {
        if (!QFileInfo::exists(file))
            continue;
        appendFacesFromFile(file, false, &faces, &seen);
    }

    s_faces = faces;
    s_cacheInvalid = false;
    KLOG_INFO(qLcFont) << "font face cache refreshed, count:" << s_faces.size();
}

void FontManager::reinitializeFontconfig()
{
    if (!ensureFontconfig())
        return;
    FcInitReinitialize();
    invalidateFontCache();
}

QSet<QString> FontManager::installedFamilyStyles()
{
    QSet<QString> keys;
    updateFontList();
    Q_FOREACH (const FontFace &face, s_faces)
        keys.insert(face.familyStyleKey());
    return keys;
}

void FontManager::resolveTargetFiles(const QList<FontInfo> &selected, QStringList *systemFiles, QStringList *userFiles)
{
    if (!systemFiles || !userFiles)
        return;

    systemFiles->clear();
    userFiles->clear();

    // 获取每行字体信息对应的字体文件
    Q_FOREACH (const FontInfo &row, selected)
    {
        Q_FOREACH (const QString &file, row.files)
        {
            if (file.isEmpty())
                continue;
            if (row.isSystem)
            {
                if (!systemFiles->contains(file))
                    *systemFiles << file;
            }
            else if (!userFiles->contains(file))
            {
                *userFiles << file;
            }
        }
    }
}

QList<FontInfo> FontManager::listFonts()
{
    updateFontList();
    return toTableRows(s_faces);
}

bool FontManager::addFonts(const QStringList &files, bool system, QString *error)
{
    if (files.isEmpty())
        return true;

    if (!ensureFontconfig())
    {
        if (error)
            *error = QObject::tr("Failed to initialize fontconfig");
        return false;
    }

    QSet<QString> installed = installedFamilyStyles();
    QStringList validFiles;

    Q_FOREACH (const QString &file, files)
    {
        if (!QFileInfo::exists(file))
        {
            if (error)
                *error = QObject::tr("Font file not found: %1").arg(file);
            return false;
        }

        // 判断是否是合法的字体文件
        QList<FontFace> faces;
        if (!queryFontFaces(file, &faces, error))
            return false;

        Q_FOREACH (const FontFace &face, faces)
        {
            if (installed.contains(face.familyStyleKey()))
            {
                if (error)
                {
                    *error = QObject::tr("Font already installed: %1, %2")
                                 .arg(face.family, face.style);
                }
                return false;
            }
        }

        Q_FOREACH (const FontFace &face, faces)
            installed.insert(face.familyStyleKey());
        validFiles << file;
    }

    if (system)
    {
        if (!PrivilegedHelper::installSystemFonts(validFiles, error))
            return false;
    }
    else
    {
        if (!FontUtils::copyFontFiles(validFiles, FontUtils::userFontsDir(), error))
            return false;
        if (!FontUtils::runFcCache(FontUtils::userFontsDir(), error))
            return false;
    }

    reinitializeFontconfig();
    return true;
}

bool FontManager::removeFonts(const QList<FontInfo> &fonts, QString *error)
{
    QStringList systemFiles;
    QStringList userFiles;
    resolveTargetFiles(fonts, &systemFiles, &userFiles);

    if (systemFiles.isEmpty() && userFiles.isEmpty())
        return true;

    if (!userFiles.isEmpty())
    {
        if (!FontUtils::removeFontFiles(userFiles, error))
            return false;
        FontUtils::removeRejectFiles(FontUtils::userRejectConfPath(), userFiles, nullptr);
        FontUtils::runFcCache(FontUtils::userFontsDir(), nullptr);
    }

    if (!systemFiles.isEmpty())
    {
        if (!PrivilegedHelper::removeSystemFonts(systemFiles, error))
            return false;
        FontUtils::removeRejectFiles(FontUtils::userRejectConfPath(), systemFiles, nullptr);
    }

    reinitializeFontconfig();
    return true;
}

bool FontManager::setFontsEnabled(const QList<FontInfo> &fonts, bool enabled, QString *error)
{
    QStringList systemFiles;
    QStringList userFiles;
    resolveTargetFiles(fonts, &systemFiles, &userFiles);

    QStringList files = userFiles;
    Q_FOREACH (const QString &file, systemFiles)
    {
        if (!files.contains(file))
            files << file;
    }

    if (files.isEmpty())
        return true;

    const QString confPath = FontUtils::userRejectConfPath();
    const bool ok = enabled ? FontUtils::removeRejectFiles(confPath, files, error)
                            : FontUtils::addRejectFiles(confPath, files, error);
    if (!ok)
        return false;

    reinitializeFontconfig();
    return true;
}
