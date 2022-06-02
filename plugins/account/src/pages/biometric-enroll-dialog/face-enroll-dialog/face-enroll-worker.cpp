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

#include "face-enroll-worker.h"
#include <qt5-log-i.h>
#include <zmq.h>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPixmap>

#define IMAGE_TYPE 0x60  //图形类型
#define AXIS_TYPE 0x61   //人脸坐标类型

FaceEnrollWorker::FaceEnrollWorker(const QString &zmqAddr, QObject *parent)
    : m_zmqAddr(zmqAddr),
      QThread(parent)
{
    QThread::setPriority(QThread::HighPriority);
}

FaceEnrollWorker::FaceEnrollWorker(QObject *parent) : QThread(parent)
{
}

FaceEnrollWorker::~FaceEnrollWorker()
{
}

void FaceEnrollWorker::setZeroMQAddr(const QString &addr)
{
    m_zmqAddr = addr;
}

void FaceEnrollWorker::run()
{
    int iRet = 0;
    int recvtimeo = 500;
    void *context, *socket;

    std::string strAddr = m_zmqAddr.toStdString();

    context = zmq_ctx_new();
    if (!context)
    {
        KLOG_WARNING() << "cmq_ctx_new failed";
        return;
    }

    socket = zmq_socket(context, ZMQ_SUB);
    zmq_setsockopt(socket, ZMQ_RCVTIMEO, &recvtimeo, sizeof(recvtimeo));
    zmq_setsockopt(socket, ZMQ_SUBSCRIBE, "", 0);
    iRet = zmq_connect(socket, strAddr.c_str());
    while (!QThread::isInterruptionRequested())
    {
        zmq_msg_t msg;
        QJsonParseError error{};

        zmq_msg_init(&msg);
        iRet = zmq_msg_recv(&msg, socket, 0);
        if (iRet == -1)
        {
            KLOG_DEBUG() << "zmq_msg_recv:" << strerror(errno);
            continue;
        }
        void *data = zmq_msg_data(&msg);
        QByteArray msgByteArray((char *)data, zmq_msg_size(&msg));
        QJsonDocument doc = QJsonDocument::fromJson(msgByteArray, &error);
        QJsonObject object = doc.object();
        int msgType = object.value("type").toInt();
        if (msgType == IMAGE_TYPE)
        {
            parseFaceImage(object);
        }
        else if (msgType == AXIS_TYPE)
        {
            parseFaceAxis(object);
        }
        zmq_msg_close(&msg);
    }
    zmq_close(socket);
    zmq_ctx_destroy(context);
}

void FaceEnrollWorker::parseFaceImage(const QJsonObject &jsonObject)
{
    int width = jsonObject["width"].toInt();
    int height = jsonObject["height"].toInt();
    QString content = jsonObject["content"].toString();
    QByteArray byteArray = QByteArray::fromBase64(content.toUtf8());
    QImage image((uchar *)byteArray.data(), width, height, QImage::Format_RGB888);
    QImage newImgae = image.rgbSwapped();
    KLOG_DEBUG() << "recv image:" << width << "x" << height;
    emit sigHasNewImage(newImgae);
}

void FaceEnrollWorker::parseFaceAxis(const QJsonObject &jsonObject)
{
    QList<QRect> res;
    QJsonValue value = jsonObject["content"];
    QString str = value.toString();
    QJsonDocument doc = QJsonDocument::fromJson(str.toLatin1());

    int count = 1;
    if (doc.isArray())
    {
        QJsonArray array = doc.array();
        for (QJsonValue item : array)
        {
            KLOG_INFO() << "item" << item;
            QJsonObject itemObject = item.toObject();
            int tlX, tlY, width, height;
            tlX = itemObject["x"].toInt();
            tlY = itemObject["y"].toInt();
            width = itemObject["h"].toInt();
            height = itemObject["w"].toInt();
            QRect rect(tlX, tlY, width, height);
            res << rect;
            KLOG_INFO() << "face " << count++ << " -- top-left:" << rect.topLeft() << rect.width() << "x" << rect.height();
        }
    }
    emit sigFaceAxis(res);
}
