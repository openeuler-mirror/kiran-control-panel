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

#include "face-enroll-dialog.h"
#include "face-enroll-worker.h"
#include "ksd_biometrics_proxy.h"
#include "ui_face-enroll-dialog.h"

#include <qt5-log-i.h>
#include <QPainter>

Q_DECLARE_METATYPE(QList<QRect>);

#define REFRESH_DELAY_TIME_MS 100

FaceEnrollDialog::FaceEnrollDialog(QWidget *parent)
    : KiranTitlebarWindow(parent),
      ui(new Ui::FaceEnrollDialog),
      m_interface(new KSDBiometricsProxy("com.kylinsec.Kiran.SystemDaemon.Biometrics",
                                         "/com/kylinsec/Kiran/SystemDaemon/Biometrics",
                                         QDBusConnection::systemBus(), this)),
      m_enrollThread(new FaceEnrollWorker(this))
{
    qRegisterMetaType<QList<QRect>>("QList<QRect>");
    ui->setupUi(getWindowContentWidget());
    init();
}

FaceEnrollDialog::~FaceEnrollDialog()
{
    stopEnroll();
    delete ui;
}

void FaceEnrollDialog::init()
{
    initUI();
    ///处理工作线程收到新图像
    connect(m_enrollThread, &FaceEnrollWorker::sigHasNewImage, this, &FaceEnrollDialog::slotHasNewImage);
    ///处理工作线程收到新的人脸坐标
    connect(m_enrollThread, &FaceEnrollWorker::sigFaceAxis, this, &FaceEnrollDialog::slotFaceAxis);
    ///连接到DBus服务获取采集状态
    connect(m_interface, &KSDBiometricsProxy::EnrollFaceStatus, this, &FaceEnrollDialog::slotUpdateEnrollFaceStatus);
    connect(ui->btn_save, &QPushButton::clicked, [this]() {
        m_isSave = true;
        close();
    });
    connect(ui->btn_cancel, &QPushButton::clicked, [this]() {
        m_isSave = false;
        close();
    });

    m_refeshDelayTimer.setInterval(REFRESH_DELAY_TIME_MS);
    connect(&m_refeshDelayTimer, &QTimer::timeout, [this]() {
        generateNewPreviewImage();
    });

    setTips(TIP_TYPE_INFO, tr("initializing face collection environment..."));

    ///开始采集
    startEnroll();
}

void FaceEnrollDialog::initUI()
{
    ///设置窗口模态
    setWindowModality(Qt::ApplicationModal);
    setWindowFlag(Qt::Dialog);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setFixedSize(444, 555);
    setButtonHints(KiranTitlebarWindow::TitlebarCloseButtonHint);
    setTitlebarColorBlockEnable(true);
}

void FaceEnrollDialog::closeEvent(QCloseEvent *event)
{
    if (!m_isSave && !m_biometricID.isEmpty())
    {
        KLOG_INFO() << "start delete enrolled face...";
        auto deleteBiometricReply = m_interface->DeleteEnrolledFace(m_biometricID);
        deleteBiometricReply.waitForFinished();
        if (deleteBiometricReply.isError())
        {
            KLOG_WARNING() << "delete biometric" << m_biometricID
                           << "     reply error:" << deleteBiometricReply.error();
        }
        KLOG_INFO() << "delete enrolled face finished...";
        m_biometricID.clear();
    }
    emit sigClose();
    QWidget::closeEvent(event);
}

bool FaceEnrollDialog::startEnroll()
{
    auto reply = m_interface->EnrollFaceStart();
    reply.waitForFinished();
    if (reply.isError())
    {
        KLOG_WARNING() << "enroll face start error:" << reply.error();
        if (reply.error().name() == "com.kylinsec.Kiran.SystemDaemon.Biometrics.Error.DeviceBusy")
        {
            KLOG_INFO() << "device is busy,stop enroll face fisrt...";
            auto stopEnrollReply = m_interface->EnrollFaceStop();
            stopEnrollReply.waitForFinished();
            if (stopEnrollReply.isError())
            {
                KLOG_WARNING() << "stop enroll face error:" << stopEnrollReply.error();
                setTips(TIP_TYPE_ERROR, tr("failed to initialize face collection environment!"));
                return false;
            }
            else
            {
                reply = m_interface->EnrollFaceStart();
                reply.waitForFinished();
                if (reply.isError())
                {
                    KLOG_WARNING() << "enroll face start error:" << reply.error();
                    QString errMsg = QString("%1(%2)")
                                         .arg(tr("Failed to start collection"))
                                         .arg(reply.error().message());
                    setTips(TIP_TYPE_ERROR, errMsg);
                    return false;
                }
            }
        }
        else
        {
            KLOG_WARNING() << "enroll face start error:" << reply.error();
            QString errMsg = QString("%1(%2)")
                                 .arg(tr("Failed to start collection"))
                                 .arg(reply.error().message());
            setTips(TIP_TYPE_ERROR, errMsg);
            return false;
        }
    }
    m_enrollThread->setZeroMQAddr(reply.value());
    m_enrollThread->start();
    m_enrollStarted = true;
    return true;
}

void FaceEnrollDialog::stopEnroll()
{
    if (m_enrollThread->isRunning())
    {
        m_enrollThread->requestInterruption();
        m_enrollThread->wait();
    }
}

void FaceEnrollDialog::slotHasNewImage(QImage image)
{
    m_tempImage = image;
    if (!m_refeshDelayTimer.isActive())
    {
        m_refeshDelayTimer.start();
    }
}

void FaceEnrollDialog::slotFaceAxis(QList<QRect> rect)
{
    m_faces = rect;
    if (!m_refeshDelayTimer.isActive())
    {
        m_refeshDelayTimer.start();
    }
}

void FaceEnrollDialog::slotUpdateEnrollFaceStatus(const QString &message, const QString &id,
                                                  int progress, bool done)
{
    if (!m_enrollStarted)
    {
        KLOG_INFO() << "enroll start failed,ignore enroll face status.";
        return;
    }

    KLOG_INFO() << "recv EnrollFaceStatus:";
    KLOG_INFO() << "    message: " << message;
    KLOG_INFO() << "    id:      " << id;
    KLOG_INFO() << "    progress:" << progress;
    KLOG_INFO() << "    done:    " << done;

    if (!message.isEmpty())
    {
        setTips(TIP_TYPE_INFO, message);
    }

    ui->enrollProgress->setProgressValue(progress);

    if (done)
    {
        m_enrollStarted = false;
        m_biometricID = id;
    }
}

void FaceEnrollDialog::generateNewPreviewImage()
{
    QPixmap drawPixmap = QPixmap::fromImage(m_tempImage);
    QPainter painter(&drawPixmap);
    QPen pen;
    pen.setColor(QColor(255, 255, 255, 255 * 0.5));
    pen.setWidthF(2);
    painter.setPen(pen);
    for (QRect &rect : m_faces)
    {
        painter.drawRect(rect);
    }
    QSize previewSize(ui->enrollProgress->getCenterImageRadius() * 2, ui->enrollProgress->getCenterImageRadius() * 2);
    ui->enrollProgress->updateCenterImage(drawPixmap.scaled(previewSize));
}

void FaceEnrollDialog::setTips(FaceEnrollDialog::TipType type, const QString &tip)
{
    QString colorText = QString("<font color=%1>%2</font>")
                            .arg(type == TIP_TYPE_INFO ? "white" : "red")
                            .arg(tip);
    ui->label_msg->setText(colorText);
}

QString FaceEnrollDialog::getFaceDataID()
{
    return m_biometricID;
}
