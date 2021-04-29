//
// Created by lxh on 2021/3/18.
//

#include "fingerprint-input-worker.h"
#include "biometrics-interface.h"
#include "log.h"

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
                LOG_INFO_S() << "finger print status:";
                LOG_INFO_S() << "    id:      " << id;
                LOG_INFO_S() << "    progress:" << progress;
                LOG_INFO_S() << "    done:    " << done;
                LOG_INFO_S() << "    message: " << message;
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
    LOG_INFO_S() << "start finger print enroll...";
    stopFingerprintEnroll();
    start();
}

void FingerprintInputWorker::stopFingerprintEnroll()
{
    LOG_INFO_S() << "stop finger print enroll...";
    if (m_started)
    {
        m_interface->EnrollFprintStop();
    }
    if (isRunning())
    {
        QThread::requestInterruption();
        QThread::wait();
    }
    LOG_INFO_S() << "stop finger print enroll finish...";
}

void FingerprintInputWorker::run()
{
    emit sigShowStatus(0, tr("initializing fingerprint collection environment..."));
    auto reply = m_interface->EnrollFprintStart();
    reply.waitForFinished();
    if (reply.isError())
    {
        LOG_INFO_S() << reply.error();
        emit sigEnrollError(reply.error().message());
        return;
    }
    m_started = true;
}
