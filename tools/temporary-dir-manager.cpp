//
// Created by lxh on 2020/10/26.
//
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMutex>
#include <QScopedPointer>
#include <QUuid>

#include "log.h"
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
    QString   temporarDirPath = QString("/tmp/%1").arg(dirName);
    QFileInfo fileInfo(temporarDirPath);

    if (m_initFinished)
    {
        return true;
    }

    if (fileInfo.exists())
    {
        QDir dir(fileInfo.absoluteFilePath());
        LOG_INFO_S() << "remove " << dir.path() << (dir.removeRecursively() ? "success" : "failed");
    }

    QDir tempDir("/tmp");
    if (tempDir.mkdir(dirName))
    {
        QFile accountTempDir(QString("/tmp/%1").arg(dirName));
        accountTempDir.setPermissions(QFile::ReadOwner|QFile::WriteOwner|
                                      QFile::ReadGroup|QFile::WriteGroup|
                                      QFile::ReadOther|QFile::WriteOther);
        m_initFinished     = true;
        m_temporaryDirPath = temporarDirPath;
        LOG_INFO_S() << "Temporary Dir Path:" << m_temporaryDirPath;
        return true;
    }

    LOG_WARNING_S() << "create temporary dir " << m_temporaryDirPath << "failed!";
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
        LOG_WARNING_S() << "not initialized,call TemporaryDirManager::init";
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
