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
#include "font-utils.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QTextStream>
#include <unistd.h>

static void printUsage()
{
    QTextStream out(stdout);
    out << "Usage: kiran-cpanel-font-helper <command> [files...]\n"
        << "Commands:\n"
        << "  install <file> [file...]   Install font files as system fonts\n"
        << "  remove  <file> [file...]   Remove system font files\n";
}

static QStringList collectFiles(int argc, char **argv, int start)
{
    QStringList files;
    for (int i = start; i < argc; ++i)
        files << QString::fromLocal8Bit(argv[i]);
    return files;
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    if (argc < 2)
    {
        printUsage();
        return 1;
    }

    if (geteuid() != 0)
    {
        QTextStream err(stderr);
        err << "this helper must be run as root via pkexec\n";
        return 1;
    }

    const QString command = QString::fromLocal8Bit(argv[1]);
    const QStringList files = collectFiles(argc, argv, 2);
    if (files.isEmpty())
    {
        printUsage();
        return 1;
    }

    QString error;
    if (command == QLatin1String("install"))
    {
        Q_FOREACH (const QString &file, files)
        {
            if (!QFileInfo::exists(file))
            {
                QTextStream(stderr) << "file not found: " << file << "\n";
                return 2;
            }
            if (!FontUtils::isFontFile(file))
            {
                QTextStream(stderr) << "unsupported font file: " << file << "\n";
                return 2;
            }
        }

        if (!FontUtils::copyFontFiles(files, FontUtils::systemInstallDir(), &error))
        {
            QTextStream(stderr) << error << "\n";
            return 3;
        }
        if (!FontUtils::runFcCache(FontUtils::systemInstallDir(), &error))
        {
            QTextStream(stderr) << error << "\n";
            return 4;
        }
        return 0;
    }

    if (command == QLatin1String("remove"))
    {
        Q_FOREACH (const QString &file, files)
        {
            if (!FontUtils::isAllowedSystemFontPath(file))
            {
                QTextStream(stderr) << "refusing to remove file outside system font directories: " << file << "\n";
                return 2;
            }
        }

        if (!FontUtils::removeFontFiles(files, &error))
        {
            QTextStream(stderr) << error << "\n";
            return 3;
        }
        FontUtils::removeRejectFiles(FontUtils::systemRejectConfPath(), files, nullptr);
        if (!FontUtils::runFcCache(QString(), &error))
        {
            QTextStream(stderr) << error << "\n";
            return 4;
        }
        return 0;
    }

    printUsage();
    return 1;
}
