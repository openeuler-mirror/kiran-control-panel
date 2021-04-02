//
// Created by lxh on 2021/3/24.
//

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
