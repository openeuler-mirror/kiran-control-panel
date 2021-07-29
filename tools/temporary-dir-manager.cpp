 /**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; If not, see <http: //www.gnu.org/licenses/>. 
  */

#include <QDir>
#include <QFileInfo>
#include <QMutex>
#include <QScopedPointer>
#include <QUuid>
#include <qt5-log-i.h>

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
        KLOG_ERROR() << "can't get $HOME!!";
        return false;
    }
    QString tempDirPrefix = QString("%1/.cache").arg(home);
    QString temporarDirPath = QString("%1/%2").arg(tempDirPrefix).arg(dirName);
    QFileInfo fileInfo(temporarDirPath);
    if (fileInfo.exists())
    {
        QDir dir(fileInfo.absoluteFilePath());
        KLOG_INFO() << "remove " << dir.path() << (dir.removeRecursively() ? "success" : "failed");
    }

    QDir tempDir(tempDirPrefix);
    if (tempDir.mkdir(dirName))
    {
        m_initFinished     = true;
        m_temporaryDirPath = temporarDirPath;
        KLOG_INFO() << "Temporary Dir Path:" << m_temporaryDirPath;
        return true;
    }

    KLOG_WARNING() << "create temporary dir " << temporarDirPath << "failed!";
    return false;
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
        KLOG_WARNING() << "not initialized,call TemporaryDirManager::init";
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
