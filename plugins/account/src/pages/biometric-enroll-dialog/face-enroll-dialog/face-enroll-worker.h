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

#ifndef KIRAN_ACCOUNT_MANAGER_FACEENROLLWORKER_H
#define KIRAN_ACCOUNT_MANAGER_FACEENROLLWORKER_H

#include <QThread>

class FaceEnrollWorker : public QThread
{
    Q_OBJECT
public:
    explicit FaceEnrollWorker(const QString &zmqAddr, QObject *parent = nullptr);
    explicit FaceEnrollWorker(QObject *parent = nullptr);
    ~FaceEnrollWorker();

    void setZeroMQAddr(const QString &addr);

signals:
    void sigHasNewImage(QImage image);
    void sigFaceAxis(QList<QRect> faceRects);

protected:
    void parseFaceImage(const QJsonObject &jsonObject);
    void parseFaceAxis(const QJsonObject &jsonObject);
    void run() override;

private:
    QString m_zmqAddr;
};

#endif  //KIRAN_ACCOUNT_MANAGER_FACEENROLLWORKER_H
