/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-appearance is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#include "image-load-manager.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QtConcurrent/QtConcurrent>

ImageLoadManager::ImageLoadManager(QObject *parent) : QObject(parent)
{
    init();
}

void ImageLoadManager::init()
{
    connect(&m_loadFutureWatcher, &QFutureWatcher<QPixmap>::finished,
            this, &ImageLoadManager::onLoadFinished, Qt::QueuedConnection);
}

ImageLoadManager *ImageLoadManager::instance()
{
    static QMutex mutex;
    static QScopedPointer<ImageLoadManager> pInst;

    if (Q_UNLIKELY(!pInst))
    {
        QMutexLocker locker(&mutex);
        if (pInst.isNull())
        {
            pInst.reset(new ImageLoadManager);
        }
    }

    return pInst.data();
}

ImageLoadManager::~ImageLoadManager()
{
    reset();
}

void ImageLoadManager::load(QString imagePath, QSize size)
{
    m_loadReqQueue.enqueue(QPair<QString, QSize>(imagePath, size));
    if (m_loadReqQueue.size() == 1)
    {
        handlerNextLoadReq();
    }
}

void ImageLoadManager::reset()
{
    m_loadReqQueue.clear();
    m_loadFutureWatcher.cancel();
    m_loadFutureWatcher.waitForFinished();
}

void ImageLoadManager::onLoadFinished()
{
    if (m_loadFutureWatcher.isCanceled())
    {
        return;
    }
    emit imageLoaded(m_loadReqQueue.front().first, m_loadReqQueue.front().second, m_loadFutureWatcher.result());
    m_loadReqQueue.pop_front();
    if (!m_loadReqQueue.empty())
    {
        handlerNextLoadReq();
    }
}

void ImageLoadManager::handlerNextLoadReq()
{
    Q_ASSERT(m_loadFutureWatcher.isFinished() || m_loadFutureWatcher.isCanceled());
    QPair<QString, QSize> req = m_loadReqQueue.front();
    QFuture<QPixmap> future = QtConcurrent::run(loadPixmap, req.first, req.second);
    m_loadFutureWatcher.setFuture(future);
}

QPixmap ImageLoadManager::loadPixmap(QString imagePath, QSize size)
{
    QPixmap pixmap;
    pixmap.load(imagePath);

    QSize pixmapSize = pixmap.size();
    qreal scaleFactor = qMax(size.width() / (double)pixmapSize.width(), size.height() / (double)pixmapSize.height());

    QSize generatePixmapSize = QSize(floor(pixmapSize.width() * scaleFactor + 0.5),
                                     floor(pixmapSize.height() * scaleFactor + 0.5));

    QPixmap scaledPixmap = pixmap.scaled(generatePixmapSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    return scaledPixmap;
}
