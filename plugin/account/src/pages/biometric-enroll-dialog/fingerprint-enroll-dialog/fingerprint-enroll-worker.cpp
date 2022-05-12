/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-cpanel-account is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#include "fingerprint-enroll-worker.h"
#include "ksd_biometrics_proxy.h"

#include <qt5-log-i.h>

FingerprintInputWorker::FingerprintInputWorker(QObject *parent)
    : QThread(parent),
      m_interface(new KSDBiometricsProxy("com.kylinsec.Kiran.SystemDaemon.Biometrics",
                                         "/com/kylinsec/Kiran/SystemDaemon/Biometrics",
                                         QDBusConnection::systemBus(), this))
{
    connect(m_interface, &KSDBiometricsProxy::EnrollFprintStatus,
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
