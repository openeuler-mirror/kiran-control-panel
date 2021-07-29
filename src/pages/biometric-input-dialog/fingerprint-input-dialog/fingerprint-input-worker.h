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
    bool m_started;  //标识采集是否由这次这次认证发出
};

#endif  //KIRAN_ACCOUNT_MANAGER_FINGERPRINT_INPUT_WORKER_H
