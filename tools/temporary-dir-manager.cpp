//
// Created by lxh on 2020/10/26.
//
#include <QMutex>
#include <QScopedPointer>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QUuid>

#include "temporary-dir-manager.h"

TemporaryDirManager::TemporaryDirManager ()
{

}

TemporaryDirManager::~TemporaryDirManager ()
{
    remove();
}

bool TemporaryDirManager::init (const QString &dirName)
{
    QString temporarDirPath = QString("/tmp/%1").arg(dirName);
    QFileInfo fileInfo(temporarDirPath);

    if (m_initFinished)
    {
        return true;
    }

    if (fileInfo.exists())
    {
        QDir dir(fileInfo.absoluteFilePath());
        qInfo() << "remove " << dir.path() << (dir.removeRecursively() ? "success" : "failed");
    }

    QDir tempDir("/tmp");
    if (tempDir.mkdir(dirName))
    {
        m_initFinished = true;
        m_temporaryDirPath = temporarDirPath;
        qInfo() << "Temporary Dir Path:" << m_temporaryDirPath;
        return true;
    }

    return false;
}

void TemporaryDirManager::remove ()
{
    QFileInfo fileInfo(m_temporaryDirPath);
    if (fileInfo.exists())
    {
        QDir dir(m_temporaryDirPath);
        dir.removeRecursively();
    }
}

QString TemporaryDirManager::generateTempFilePath ()
{
    if (!m_initFinished)
    {
        qWarning() << "not initialized,call TemporaryDirManager::init";
        return QString("");
    }
    QUuid id = QUuid::createUuid();
    return QString("%1/%2").arg(m_temporaryDirPath).arg(id.toString(QUuid::WithoutBraces));
}

TemporaryDirManager *TemporaryDirManager::instance ()
{
    static QMutex mutex;
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
