/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#ifndef KIRAN_ACCOUNT_MANAGER_FINGERPRINT_INPUT_WORKER_H
#define KIRAN_ACCOUNT_MANAGER_FINGERPRINT_INPUT_WORKER_H

#include <QThread>

class KSDBiometricsProxy;
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
    KSDBiometricsProxy *m_interface;
    bool m_started;  //标识采集是否由这次这次认证发出
};

#endif  //KIRAN_ACCOUNT_MANAGER_FINGERPRINT_INPUT_WORKER_H
