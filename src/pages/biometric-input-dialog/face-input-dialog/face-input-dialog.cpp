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
 

#include "face-input-dialog.h"
#include "biometrics-interface.h"
#include "face-enroll-worker.h"
#include "ui_face-input-dialog.h"

#include <QPainter>
#include <qt5-log-i.h>

Q_DECLARE_METATYPE(QList<QRect>);

#define REFRESH_DELAY_TIME_MS 100

FaceInputDialog::FaceInputDialog(QWidget *parent)
    : KiranTitlebarWindow(parent),
      ui(new Ui::FaceInputDialog),
      m_interface(new BiometricsInterface(QDBusConnection::systemBus(), this)),
      m_enrollThread(new FaceEnrollWorker(this))
{
    qRegisterMetaType<QList<QRect>>("QList<QRect>");
    ui->setupUi(getWindowContentWidget());
    init();
}

FaceInputDialog::~FaceInputDialog()
{
    stopEnroll();
    delete ui;
}

void FaceInputDialog::init()
{
    initUI();
    ///处理工作线程收到新图像
    connect(m_enrollThread, &FaceEnrollWorker::sigHasNewImage, this, &FaceInputDialog::slotHasNewImage);
    ///处理工作线程收到新的人脸坐标
    connect(m_enrollThread, &FaceEnrollWorker::sigFaceAxis, this, &FaceInputDialog::slotFaceAxis);
    ///连接到DBus服务获取采集状态
    connect(m_interface, &BiometricsInterface::EnrollFaceStatus, this, &FaceInputDialog::slotUpdateEnrollFaceStatus);
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

void FaceInputDialog::initUI()
{
    ///设置窗口模态
    setWindowModality(Qt::ApplicationModal);
    setWindowFlag(Qt::Dialog);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setFixedSize(444, 555);
    setButtonHints(KiranTitlebarWindow::TitlebarCloseButtonHint);
}

void FaceInputDialog::closeEvent(QCloseEvent *event)
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

bool FaceInputDialog::startEnroll()
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

void FaceInputDialog::stopEnroll()
{
    if (m_enrollThread->isRunning())
    {
        m_enrollThread->requestInterruption();
        m_enrollThread->wait();
    }
}

void FaceInputDialog::slotHasNewImage(QImage image)
{
    m_tempImage = image;
    if (!m_refeshDelayTimer.isActive())
    {
        m_refeshDelayTimer.start();
    }
}

void FaceInputDialog::slotFaceAxis(QList<QRect> rect)
{
    m_faces = rect;
    if (!m_refeshDelayTimer.isActive())
    {
        m_refeshDelayTimer.start();
    }
}

void FaceInputDialog::slotUpdateEnrollFaceStatus(const QString &message, const QString &id,
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

void FaceInputDialog::generateNewPreviewImage()
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

void FaceInputDialog::setTips(FaceInputDialog::TipType type, const QString &tip)
{
    QString colorText = QString("<font color=%1>%2</font>")
                            .arg(type == TIP_TYPE_INFO ? "white" : "red")
                            .arg(tip);
    ui->label_msg->setText(colorText);
}

QString FaceInputDialog::getFaceDataID()
{
    return m_biometricID;
}
