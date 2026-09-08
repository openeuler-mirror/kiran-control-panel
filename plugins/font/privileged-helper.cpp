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
#include "privileged-helper.h"
#include "config.h"
#include "logging-category.h"

#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QProcess>
#include <QTemporaryDir>

bool PrivilegedHelper::installSystemFonts(const QStringList &files, QString *error)
{
    QTemporaryDir tempDir;
    if (!tempDir.isValid())
    {
        if (error)
            *error = QObject::tr("Failed to create a temporary directory.");
        return false;
    }

    QStringList stagedFiles;
    for (int i = 0; i < files.size(); ++i)
    {
        const QFileInfo srcInfo(files.at(i));
        QString destName = srcInfo.fileName();
        QString destPath = tempDir.filePath(destName);
        if (QFile::exists(destPath))
        {
            destName = QString("%1-%2.%3").arg(srcInfo.completeBaseName()).arg(i).arg(srcInfo.suffix());
            destPath = tempDir.filePath(destName);
        }
        if (!QFile::copy(files.at(i), destPath))
        {
            if (error)
                *error = QObject::tr("Failed to stage font file: %1").arg(files.at(i));
            return false;
        }
        QFile::setPermissions(destPath, QFile::ReadOwner | QFile::WriteOwner | QFile::ReadGroup | QFile::ReadOther);
        stagedFiles << destPath;
    }

    return run(QStringList() << QStringLiteral("install") << stagedFiles, error);
}

bool PrivilegedHelper::removeSystemFonts(const QStringList &files, QString *error)
{
    return run(QStringList() << QStringLiteral("remove") << files, error);
}

bool PrivilegedHelper::run(const QStringList &args, QString *error)
{
    if (!QFileInfo::exists(QStringLiteral(FONT_HELPER_PATH)))
    {
        if (error)
            *error = QObject::tr("Font helper is not installed: %1").arg(QStringLiteral(FONT_HELPER_PATH));
        return false;
    }

    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);

    QStringList cmd;
    cmd << QStringLiteral(FONT_HELPER_PATH) << args;
    process.start(QStringLiteral("pkexec"), cmd);
    if (!process.waitForStarted(15000))
    {
        KLOG_ERROR(qLcFont) << "failed to start pkexec";
        if (error)
            *error = QObject::tr("Failed to start pkexec.");
        return false;
    }

    if (!process.waitForFinished(-1))
    {
        KLOG_ERROR(qLcFont) << "pkexec timed out";
        if (error)
            *error = QObject::tr("Privileged operation timed out.");
        return false;
    }

    const int exitCode = process.exitCode();
    const QString output = QString::fromUtf8(process.readAll()).trimmed();
    if (process.exitStatus() != QProcess::NormalExit || exitCode != 0)
    {
        KLOG_WARNING(qLcFont) << "privileged helper failed, code:" << exitCode << output;
        if (error)
        {
            if (exitCode == 126 || exitCode == 127)
                *error = QObject::tr("Authorization was cancelled or pkexec is unavailable.");
            else if (!output.isEmpty())
                *error = output;
            else
                *error = QObject::tr("Privileged operation failed.");
        }
        return false;
    }

    return true;
}
