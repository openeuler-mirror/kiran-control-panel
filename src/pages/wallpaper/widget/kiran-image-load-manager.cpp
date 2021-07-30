//
// Created by lxh on 2021/1/15.
//

#include "kiran-image-load-manager.h"

#include <QStandardPaths>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QtConcurrent/QtConcurrent>
#include <iostream>

KiranImageLoadManager::KiranImageLoadManager(QObject *parent) : QObject(parent) {
    init();
}

void KiranImageLoadManager::init() {
    connect(&m_loadFutureWatcher, &QFutureWatcher<QPixmap>::finished,
            this, &KiranImageLoadManager::onLoadFinished, Qt::QueuedConnection);
}

KiranImageLoadManager *KiranImageLoadManager::instance() {
    static QMutex mutex;
    static QScopedPointer<KiranImageLoadManager> pInst;

    if (Q_UNLIKELY(!pInst)) {
        QMutexLocker locker(&mutex);
        if (pInst.isNull()) {
            pInst.reset(new KiranImageLoadManager);
        }
    }

    return pInst.data();
}

KiranImageLoadManager::~KiranImageLoadManager() {
    reset();
}

void KiranImageLoadManager::load(QString imagePath, QSize size) {
    m_loadReqQueue.enqueue(QPair<QString, QSize>(imagePath, size));
    if (m_loadReqQueue.size() == 1) {
        handlerNextLoadReq();
    }
}

void KiranImageLoadManager::reset() {
    m_loadReqQueue.clear();
    m_loadFutureWatcher.cancel();
    m_loadFutureWatcher.waitForFinished();
}

void KiranImageLoadManager::onLoadFinished() {
    if (m_loadFutureWatcher.isCanceled()) {
        return;
    }
    emit imageLoaded(m_loadReqQueue.front().first, m_loadReqQueue.front().second, m_loadFutureWatcher.result());
    m_loadReqQueue.pop_front();
    if (!m_loadReqQueue.empty()) {
        handlerNextLoadReq();
    }
}

void KiranImageLoadManager::handlerNextLoadReq() {
    Q_ASSERT(m_loadFutureWatcher.isFinished() || m_loadFutureWatcher.isCanceled());
    QPair<QString, QSize> req = m_loadReqQueue.front();
    QFuture<QPixmap> future = QtConcurrent::run(loadPixmap, req.first, req.second);
    m_loadFutureWatcher.setFuture(future);
}

QPixmap KiranImageLoadManager::loadPixmap(QString imagePath, QSize size) {
    QPixmap pixmap;
    pixmap.load(imagePath);

//    std::cout << "imagePath:" << imagePath.toStdString()<< std::endl;

    QSize pixmapSize = pixmap.size();
    qreal scaleFactor = qMax(size.width() / (double) pixmapSize.width(), size.height() / (double) pixmapSize.height());

    QSize generatePixmapSize = QSize(floor(pixmapSize.width() * scaleFactor + 0.5),
                                     floor(pixmapSize.height() * scaleFactor + 0.5));

    QPixmap scaledPixmap = pixmap.scaled(generatePixmapSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    return scaledPixmap;
}
