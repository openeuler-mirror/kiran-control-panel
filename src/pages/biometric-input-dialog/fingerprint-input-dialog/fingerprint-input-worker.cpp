//
// Created by lxh on 2021/3/18.
//

#include "fingerprint-input-worker.h"
#include "biometrics-interface.h"

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
                qInfo() << "finger print status:";
                qInfo() << "    id:      " << id;
                qInfo() << "    progress:" << progress;
                qInfo() << "    done:    " << done;
                qInfo() << "    message: " << message;
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
    qInfo() << "start finger print enroll...";
    stopFingerprintEnroll();
    start();
}

void FingerprintInputWorker::stopFingerprintEnroll()
{
    qInfo() << "stop finger print enroll...";
    if (m_started)
    {
        m_interface->EnrollFprintStop();
    }
    if (isRunning())
    {
        QThread::requestInterruption();
        QThread::wait();
    }
    qInfo() << "stop finger print enroll finish...";
}

void FingerprintInputWorker::run()
{
    emit sigShowStatus(0, tr("initializing fingerprint collection environment..."));
    auto reply = m_interface->EnrollFprintStart();
    reply.waitForFinished();
    if (reply.isError())
    {
        qInfo() << reply.error();
        emit sigEnrollError(reply.error().message());
        return;
    }
    m_started = true;
}
