/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#include <QDir>
#include <QFileInfo>
#include <QMutex>
#include <QScopedPointer>
#include <QUuid>
#include <qt5-log-i.h>

#include "logging-category.h"
#include "temporary-dir-manager.h"

TemporaryDirManager::TemporaryDirManager()
{
}

TemporaryDirManager::~TemporaryDirManager()
{
    remove();
}

bool TemporaryDirManager::init(const QString &dirName)
{
    if (m_initFinished)
    {
        return true;
    }

    char * home = getenv("HOME");
    if( !home )
    {
        KLOG_ERROR(qLcAccount) << "can't get env(HOME)";
        return false;
    }
    QString tempDirPrefix = QString("%1/.cache").arg(home);
    QString temporarDirPath = QString("%1/%2").arg(tempDirPrefix).arg(dirName);
    QFileInfo fileInfo(temporarDirPath);
    if (fileInfo.exists())
    {
        QDir dir(fileInfo.absoluteFilePath());
        KLOG_DEBUG(qLcAccount) << "remove " << dir.path() << (dir.removeRecursively() ? "success" : "failed");
    }

    QDir tempDir(tempDirPrefix);
    if (!tempDir.mkdir(dirName))
    {
        KLOG_WARNING(qLcAccount) << "create temporary dir " << temporarDirPath << "failed!";
        return false;
    }

    m_initFinished     = true;
    m_temporaryDirPath = temporarDirPath;
    KLOG_DEBUG(qLcAccount) << "temporary dir path:" << m_temporaryDirPath;
    return true;
}

void TemporaryDirManager::remove()
{
    QFileInfo fileInfo(m_temporaryDirPath);
    if (fileInfo.exists())
    {
        QDir dir(m_temporaryDirPath);
        dir.removeRecursively();
    }
}

QString TemporaryDirManager::generateTempFilePath()
{
    if (!m_initFinished)
    {
        KLOG_WARNING(qLcAccount) << "temp dir manager not initialized,call TemporaryDirManager::init";
        return QString("");
    }
    QUuid id = QUuid::createUuid();
#if (QT_VERSION > QT_VERSION_CHECK(5,9,7))
    return QString("%1/%2").arg(m_temporaryDirPath).arg(id.toString(QUuid::WithoutBraces));
#else
    QString uuidString = id.toString();
    uuidString.remove("{");
    uuidString.remove("}");
    return QString("%1/%2").arg(m_temporaryDirPath).arg(uuidString);
#endif
}

TemporaryDirManager *TemporaryDirManager::instance()
{
    static QMutex                              mutex;
    static QScopedPointer<TemporaryDirManager> pInst;

    if (Q_UNLIKELY(!pInst))
    {
        QMutexLocker locker(&mutex);
        if (pInst.isNull())
        {
            pInst.reset(new TemporaryDirManager);
        }
    }

    return pInst.data();
}
