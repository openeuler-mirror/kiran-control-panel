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
 
#include "fingerprint-input-worker.h"
#include "biometrics-interface.h"

#include <qt5-log-i.h>

FingerprintInputWorker::FingerprintInputWorker(QObject *parent)
    : QThread(parent),
      m_interface(new BiometricsInterface(QDBusConnection::systemBus(), this))
{
    connect(m_interface, &BiometricsInterface::EnrollFprintStatus,
            [this](const QString &message, const QString &id, int progress, bool done) {
                if (!m_started)
                {
                    return;
                }
                KLOG_INFO() << "finger print status:";
                KLOG_INFO() << "    id:      " << id;
                KLOG_INFO() << "    progress:" << progress;
                KLOG_INFO() << "    done:    " << done;
                KLOG_INFO() << "    message: " << message;
                if (done)
                {
                    emit sigEnrollComplete(progress == 100, message, id);
                    m_started = false;
                }
                else
                {
                    emit sigShowStatus(progress, message);
                }
            });
}

FingerprintInputWorker::~FingerprintInputWorker()
{
    stopFingerprintEnroll();
}

void FingerprintInputWorker::startFingerprintEnroll()
{
    KLOG_INFO() << "start finger print enroll...";
    stopFingerprintEnroll();
    start();
}

void FingerprintInputWorker::stopFingerprintEnroll()
{
    KLOG_INFO() << "stop finger print enroll...";
    if (m_started)
    {
        m_interface->EnrollFprintStop();
    }
    if (isRunning())
    {
        QThread::requestInterruption();
        QThread::wait();
    }
    KLOG_INFO() << "stop finger print enroll finish...";
}

void FingerprintInputWorker::run()
{
    emit sigShowStatus(0, tr("initializing fingerprint collection environment..."));
    auto reply = m_interface->EnrollFprintStart();
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_INFO() << reply.error();
        emit sigEnrollError(reply.error().message());
        return;
    }
    m_started = true;
}
