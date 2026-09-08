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
 */
#include "font-utils.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <fontconfig/fontconfig.h>

// 系统字体安装目录、系统 fontconfig 配置目录
#define SYSTEM_INSTALL_DIR "/usr/local/share/fonts"
#define SYSTEM_CONF_DIR "/etc/fonts/conf.d"
#define REJECT_CONF_NAME "99-kcp-font-reject.conf"

QString FontUtils::systemInstallDir()
{
    return QStringLiteral(SYSTEM_INSTALL_DIR);
}

QString FontUtils::userFontsDir()
{
    // ~/.local/share/fonts  （参考fontconfig官方文档：$XDG_DATA_HOME）
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/fonts");
}

QString FontUtils::userRejectConfPath()
{
    // $XDG_CONFIG_HOME/fontconfig/conf.d/$REJECT_CONF_NAME （参考fontconfig官方文档：$XDG_CONFIG_HOME）
    return QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) +
           QStringLiteral("/fontconfig/conf.d/") + QStringLiteral(REJECT_CONF_NAME);
}

QString FontUtils::systemRejectConfPath()
{
    return QStringLiteral(SYSTEM_CONF_DIR) + QLatin1Char('/') + QStringLiteral(REJECT_CONF_NAME);
}

bool FontUtils::isFontFile(const QString &path)
{
    static const QStringList suffixes = {
        QStringLiteral("ttf"), QStringLiteral("otf"), QStringLiteral("ttc"),
        QStringLiteral("otc"), QStringLiteral("woff"), QStringLiteral("woff2"),
        QStringLiteral("pcf"), QStringLiteral("pcf.gz"), QStringLiteral("bdf"),
        QStringLiteral("pfa"), QStringLiteral("pfb")};

    const QString suffix = QFileInfo(path).suffix().toLower();
    if (suffixes.contains(suffix))
        return true;

    const QString fileName = QFileInfo(path).fileName().toLower();
    return fileName.endsWith(QStringLiteral(".pcf.gz"));
}

QStringList FontUtils::systemFontDirPrefixes()
{
    // 删除系统字体时的白名单：取自 fontconfig 已配置的字体目录，排除用户目录
    QStringList dirs;
    if (!FcInit())
        return dirs;

    FcStrList *list = FcConfigGetFontDirs(nullptr);
    if (!list)
        return dirs;

    const QString home = QDir::homePath();
    while (const FcChar8 *fcDir = FcStrListNext(list))
    {
        QString dir = QString::fromLocal8Bit(reinterpret_cast<const char *>(fcDir));
        if (dir.startsWith(QLatin1Char('~')))
            dir = home + dir.mid(1);
        dir = QDir::cleanPath(dir);
        if (dir.isEmpty() || dir == home || dir.startsWith(home + QLatin1Char('/')))
            continue;
        if (!dirs.contains(dir))
            dirs.append(dir);
    }
    FcStrListDone(list);
    return dirs;
}

bool FontUtils::isSystemFontPath(const QString &path)
{
    const QString home = QDir::homePath();
    const QString absPath = QFileInfo(path).absoluteFilePath();
    if (absPath.startsWith(home + QLatin1Char('/')) || absPath == home)
        return false;
    return true;
}

bool FontUtils::isAllowedSystemFontPath(const QString &path)
{
    const QString canonical = QFileInfo(path).canonicalFilePath();
    if (canonical.isEmpty() || !QFileInfo(canonical).isFile())
        return false;

    Q_FOREACH (const QString &prefix, systemFontDirPrefixes())
    {
        if (canonical == prefix)
            return false;
        if (canonical.startsWith(prefix + QLatin1Char('/')))
            return true;
    }
    return false;
}

QSet<QString> FontUtils::readRejectList(const QString &confPath)
{
    QSet<QString> files;
    QFile file(confPath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
        return files;

    QXmlStreamReader xml(&file);
    while (!xml.atEnd())
    {
        xml.readNext();
        if (xml.isStartElement() && xml.name() == QLatin1String("glob"))
        {
            const QString text = xml.readElementText().trimmed();
            if (!text.isEmpty())
                files.insert(text);
        }
    }
    return files;
}

bool FontUtils::writeRejectList(const QString &confPath, const QSet<QString> &files, QString *error)
{
    QFileInfo info(confPath);
    if (!QDir().mkpath(info.absolutePath()))
    {
        if (error)
            *error = QString("failed to create directory: %1").arg(info.absolutePath());
        return false;
    }

    QFile file(confPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        if (error)
            *error = QString("failed to write %1").arg(confPath);
        return false;
    }

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
    xml.writeStartDocument();
    xml.writeDTD(QStringLiteral("<!DOCTYPE fontconfig SYSTEM \"urn:fontconfig:fonts.dtd\">"));
    xml.writeStartElement(QStringLiteral("fontconfig"));
    xml.writeTextElement(QStringLiteral("description"),
                         QStringLiteral("Disabled fonts managed by Kiran Control Panel"));
    xml.writeStartElement(QStringLiteral("selectfont"));
    xml.writeStartElement(QStringLiteral("rejectfont"));

    QStringList sorted = files.values();
    sorted.sort();
    Q_FOREACH (const QString &path, sorted)
    {
        xml.writeTextElement(QStringLiteral("glob"), path);
    }

    xml.writeEndElement();  // rejectfont
    xml.writeEndElement();  // selectfont
    xml.writeEndElement();  // fontconfig
    xml.writeEndDocument();
    return true;
}

bool FontUtils::addRejectFiles(const QString &confPath, const QStringList &files, QString *error)
{
    QSet<QString> current = readRejectList(confPath);
    Q_FOREACH (const QString &file, files)
    {
        if (!file.isEmpty())
            current.insert(file);
    }
    return writeRejectList(confPath, current, error);
}

bool FontUtils::removeRejectFiles(const QString &confPath, const QStringList &files, QString *error)
{
    QSet<QString> current = readRejectList(confPath);
    Q_FOREACH (const QString &file, files)
    {
        current.remove(file);
    }
    return writeRejectList(confPath, current, error);
}

bool FontUtils::copyFontFiles(const QStringList &srcFiles, const QString &destDir, QString *error)
{
    if (!QDir().mkpath(destDir))
    {
        if (error)
            *error = QString("failed to create directory: %1").arg(destDir);
        return false;
    }

    Q_FOREACH (const QString &src, srcFiles)
    {
        QFileInfo srcInfo(src);
        if (!srcInfo.isFile())
        {
            if (error)
                *error = QString("invalid font file: %1").arg(src);
            return false;
        }

        const QString dest = destDir + QLatin1Char('/') + srcInfo.fileName();
        if (QFile::exists(dest) && !QFile::remove(dest))
        {
            if (error)
                *error = QString("failed to overwrite: %1").arg(dest);
            return false;
        }
        if (!QFile::copy(src, dest))
        {
            if (error)
                *error = QString("failed to copy %1 to %2").arg(src, dest);
            return false;
        }
    }
    return true;
}

bool FontUtils::removeFontFiles(const QStringList &files, QString *error)
{
    Q_FOREACH (const QString &file, files)
    {
        QFileInfo info(file);
        const QString canonical = info.canonicalFilePath();
        if (canonical.isEmpty())
            continue;
        if (!QFile::remove(canonical))
        {
            if (error)
                *error = QString("failed to remove: %1").arg(canonical);
            return false;
        }
    }
    return true;
}

bool FontUtils::runFcCache(const QString &dir, QString *error)
{
    QProcess process;
    QStringList args;
    args << QStringLiteral("-f");
    if (!dir.isEmpty())
        args << dir;

    process.start(QStringLiteral("fc-cache"), args);
    if (!process.waitForFinished(120000))
    {
        if (error)
            *error = QStringLiteral("fc-cache timed out");
        return false;
    }
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0)
    {
        if (error)
            *error = QString::fromUtf8(process.readAllStandardError());
        return false;
    }
    return true;
}
