/**
 *@file  fingerprint-input-worker.h
 *@brief 
 *@auth  liuxinhao <liuxinhao@kylinos.com.cn>
 *@copyright (c) 2021 KylinSec. All rights reserved.
 */
#ifndef KIRAN_ACCOUNT_MANAGER_FINGERPRINT_INPUT_WORKER_H
#define KIRAN_ACCOUNT_MANAGER_FINGERPRINT_INPUT_WORKER_H

#include <QThread>

class BiometricsInterface;

class FingerprintInputWorker : public QThread
{
    Q_OBJECT
public:
    FingerprintInputWorker(QObject *parent = nullptr);
    ~FingerprintInputWorker();

public:
    void startFingerprintEnroll();
    void stopFingerprintEnroll();

signals:
    void sigShowStatus(unsigned int progress, const QString &msg);
    void sigEnrollComplete(bool isSuccess, const QString &msg, const QString &id);
    void sigEnrollError(const QString &errMsg);

protected:
    void run() override;

private:
    BiometricsInterface *m_interface;
    bool                 m_started;  //标识采集是否由这次这次认证发出
};

#endif  //KIRAN_ACCOUNT_MANAGER_FINGERPRINT_INPUT_WORKER_H
