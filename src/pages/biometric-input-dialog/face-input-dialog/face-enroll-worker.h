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
