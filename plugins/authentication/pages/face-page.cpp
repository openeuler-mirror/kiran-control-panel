/**
 * Copyright (c) 2020 ~ 2023 KylinSec Co., Ltd.
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
#include "face-page.h"
#include "logging-category.h"
#include "utils/auth-setting-container.h"
#include "utils/auth-setting-item.h"
#include "utils/general-bio-page.h"
#include "utils/kiran-auth-dbus-proxy.h"

#include <kiran-message-box.h>
#include <qt5-log-i.h>
#include <QCamera>
#include <QCameraImageCapture>
#include <QCameraViewfinder>
#include <QVideoProbe>
#include <QBoxLayout>
#include <QBuffer>
#include <QEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QStackedWidget>
#include <QTimer>

#define PAGE_INDEX_FEATURE_MANAGER 0
#define PAGE_INDEX_FEATURE_ENROLL 1
#define ENROLL_PREVIEW_IDLE_TIMEOUT_MS 120000  // 预览 2 分钟无操作自动取消(FR-017)
#define FACE_ENROLL_IMAGE_MAX_WIDTH 640   // 提交图像最大宽度(FR-003)
#define FACE_ENROLL_IMAGE_MAX_HEIGHT 480  // 提交图像最大高度(FR-003)
#define FACE_ENROLL_JPEG_QUALITY 85       // 提交图像 JPEG 压缩质量(FR-003)

FacePage::FacePage(KiranAuthDBusProxy* proxy, QWidget* parent)
    : QWidget(parent),
      m_proxy(proxy)
{
    initUI();
    connect(m_proxy, &KiranAuthDBusProxy::EnrollStatus, this, &FacePage::updateEnrollStatus);

    m_idleTimer = new QTimer(this);
    m_idleTimer->setInterval(ENROLL_PREVIEW_IDLE_TIMEOUT_MS);
    m_idleTimer->setSingleShot(true);
    connect(m_idleTimer, &QTimer::timeout, this, &FacePage::onIdleTimeout);
}

bool FacePage::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_viewfinder && event->type() == QEvent::Resize)
    {
        m_previewMask->setGeometry(m_viewfinder->rect());
    }
    return QWidget::eventFilter(watched, event);
}

FacePage::~FacePage()
{
    if (m_inEnroll)
    {
        m_proxy->stopEnroll();
    }
    stopCamera();
}

void FacePage::initUI()
{
    auto mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(0);

    m_stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(m_stackedWidget);

    m_stackedWidget->insertWidget(PAGE_INDEX_FEATURE_MANAGER, initFeatureManagerPage());
    m_stackedWidget->insertWidget(PAGE_INDEX_FEATURE_ENROLL, initFeatureEnrollPage());
}

QWidget* FacePage::initFeatureManagerPage()
{
    m_featureManager = new GeneralBioPage(m_proxy, KAD_AUTH_TYPE_FACE, this);
    m_featureManager->setFeatureNamePrefix(tr("face"));
    m_featureManager->setDefaultDeviceLabelDesc(tr("Default face device"));
    m_featureManager->setDeviceFeatureListDesc(tr("face feature list"));
    connect(m_featureManager, &GeneralBioPage::enrollFeature, this, &FacePage::startEnrollFeature);

    return m_featureManager;
}

QWidget* FacePage::initFeatureEnrollPage()
{
    auto featureEnrollWidget = new QWidget();
    auto featureEnrollLayout = new QBoxLayout(QBoxLayout::TopToBottom, featureEnrollWidget);
    featureEnrollLayout->setSpacing(0);
    featureEnrollLayout->setContentsMargins(0, 0, 0, 16);

    // 摄像头实时预览区域:填满布局单元,随窗口缩放(最小 272x272)
    m_viewfinder = new QCameraViewfinder(featureEnrollWidget);
    m_viewfinder->setMinimumSize(272, 272);
    m_viewfinder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // stretch=1:多余垂直空间全部给预览区(下方不再放 addStretch,否则
    // 空间会被 stretch 项吃掉导致预览区高度不扩展)
    featureEnrollLayout->addWidget(m_viewfinder, 1);

    // 黑遮罩:首帧到达前覆盖预览区,防止透出上一页残留 UI;
    // QVideoProbe 探测到首帧后摘除;遮罩尺寸随预览区缩放(见 eventFilter)
    m_previewMask = new QLabel(m_viewfinder);
    m_previewMask->setGeometry(m_viewfinder->rect());
    m_previewMask->setStyleSheet("background-color: black;");
    m_previewMask->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_previewMask->hide();
    m_viewfinder->installEventFilter(this);

    featureEnrollLayout->addSpacerItem(new QSpacerItem(10, 16, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_enrollTips = new QLabel;
    m_enrollTips->setAlignment(Qt::AlignHCenter);
    featureEnrollLayout->addWidget(m_enrollTips);

    // 提示文字与按钮行之间的间距
    featureEnrollLayout->addSpacerItem(new QSpacerItem(10, 16, QSizePolicy::Minimum, QSizePolicy::Fixed));

    auto buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    buttonLayout->addStretch();

    m_captureButton = new QPushButton(tr("Capture"), featureEnrollWidget);
    m_captureButton->setFixedSize(110, 40);
    buttonLayout->addWidget(m_captureButton);
    connect(m_captureButton, &QPushButton::clicked, this, &FacePage::onCaptureClicked);

    buttonLayout->addSpacing(16);

    auto cancelButton = new QPushButton(tr("Cancel"), featureEnrollWidget);
    cancelButton->setFixedSize(110, 40);
    buttonLayout->addWidget(cancelButton);
    connect(cancelButton, &QPushButton::clicked, this, &FacePage::enrollCancel);

    buttonLayout->addStretch();
    featureEnrollLayout->addLayout(buttonLayout);

    return featureEnrollWidget;
}

void FacePage::startCamera()
{
    if (!m_camera)
    {
        m_camera = new QCamera(this);

        // 限定预览分辨率与帧率,避免后端默认高分辨率导致预览卡顿;
        // 请求 RGB32:后端转换时拷贝到自有缓冲,绘制不再直读 V4L2 mmap
        // 缓冲,避免快速运动时的画面撕裂(MJPEG 在 Qt5 V4L2 后端不可用)
        QCameraViewfinderSettings viewfinderSettings;
        viewfinderSettings.setResolution(QSize(640, 480));
        viewfinderSettings.setMinimumFrameRate(30.0);
        viewfinderSettings.setMaximumFrameRate(30.0);
        viewfinderSettings.setPixelFormat(QVideoFrame::Format_RGB32);
        m_camera->setViewfinderSettings(viewfinderSettings);
        m_imageCapture = new QCameraImageCapture(m_camera, this);
        // 只取回调图像,不落盘
        m_imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);
        connect(m_imageCapture, &QCameraImageCapture::imageCaptured, this, &FacePage::onImageCaptured);
        connect(m_imageCapture, QOverload<int, QCameraImageCapture::Error, const QString&>::of(&QCameraImageCapture::error),
                this, &FacePage::onImageCaptureError);
        connect(m_camera, QOverload<QCamera::Error>::of(&QCamera::error), this, [this](QCamera::Error error)
                {
                    KLOG_WARNING(qLcAuthentication) << "face camera error:" << error;
                    m_enrollTips->setText(tr("Camera error, please check the camera"));
                    m_captureButton->setEnabled(false);
                });
        m_camera->setViewfinder(m_viewfinder);

        m_videoProbe = new QVideoProbe(this);
        if (m_videoProbe->setSource(m_camera))
        {
            connect(m_videoProbe, &QVideoProbe::videoFrameProbed, this,
                    [this](const QVideoFrame &frame)
                    {
                        Q_UNUSED(frame);
                        m_previewMask->hide();
                    });
        }
    }
    // 每次进入先盖黑遮罩,首帧到达后自动摘除
    m_previewMask->show();
    m_previewMask->raise();
    m_captureButton->setEnabled(true);
    m_enrollTips->setText(tr("Please face the camera and click Capture"));
    m_camera->start();

    const auto applied = m_camera->viewfinderSettings();
    KLOG_INFO(qLcAuthentication) << "camera viewfinder settings applied:"
                                 << "resolution=" << applied.resolution()
                                 << "fps=" << applied.minimumFrameRate() << "-" << applied.maximumFrameRate()
                                 << "pixelFormat=" << applied.pixelFormat();
}

void FacePage::stopCamera()
{
    if (m_camera)
    {
        m_camera->stop();
        m_previewMask->hide();
    }
}

void FacePage::startEnrollFeature()
{
    KLOG_INFO(qLcAuthentication) << "start enroll face feature";
    QString featureName = m_featureManager->autoGenerateFeatureName();

    m_inEnroll = false;
    m_capturing = false;
    m_inErollFeatureName = featureName;

    // 时序调整:进入录入页即开预览,拍照时才建立服务端会话(契约 2.1)
    m_stackedWidget->setCurrentIndex(PAGE_INDEX_FEATURE_ENROLL);
    startCamera();
    m_idleTimer->start();
}

void FacePage::onCaptureClicked()
{
    KLOG_INFO(qLcAuthentication) << "capture face image";

    if (m_capturing || m_inEnroll)
    {
        return;
    }
    m_capturing = true;
    m_captureButton->setEnabled(false);
    m_enrollTips->setText(tr("Capturing..."));
    m_imageCapture->capture();
}

void FacePage::onImageCaptured(int id, const QImage& preview)
{
    Q_UNUSED(id);
    KLOG_INFO(qLcAuthentication) << "face image captured, size:" << preview.size();

    m_capturing = false;
    if (m_inEnroll)
    {
        return;
    }
    if (preview.isNull())
    {
        m_enrollTips->setText(tr("Capture failed, please try again"));
        m_captureButton->setEnabled(true);
        return;
    }
    submitEnroll(preview);
}

void FacePage::onImageCaptureError(int id, int error, const QString& errorString)
{
    Q_UNUSED(id);
    Q_UNUSED(error);
    KLOG_WARNING(qLcAuthentication) << "face image capture error:" << errorString;

    m_capturing = false;
    m_enrollTips->setText(tr("Capture failed,%1").arg(errorString));
    m_captureButton->setEnabled(true);
}

void FacePage::submitEnroll(const QImage& image)
{
    // 当前帧 → 缩放(上限 640x480)→ JPEG 压缩(质量 85) → base64 →
    // extraInfo {"faceImage": ...}(契约 2.1);单条 D-Bus 消息上限 128MB,
    // 压缩后图像约几十 KB~数百 KB,无超限风险
    QImage scaled = image;
    if (scaled.width() > FACE_ENROLL_IMAGE_MAX_WIDTH || scaled.height() > FACE_ENROLL_IMAGE_MAX_HEIGHT)
    {
        scaled = scaled.scaled(FACE_ENROLL_IMAGE_MAX_WIDTH, FACE_ENROLL_IMAGE_MAX_HEIGHT,
                               Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    QByteArray jpeg;
    QBuffer buffer(&jpeg);
    buffer.open(QIODevice::WriteOnly);
    scaled.save(&buffer, "JPG", FACE_ENROLL_JPEG_QUALITY);
    buffer.close();

    QJsonObject rootObject;
    rootObject.insert("faceImage", QString::fromLatin1(jpeg.toBase64()));
    QJsonDocument doc(rootObject);
    QString extraInfo = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    KLOG_INFO(qLcAuthentication) << "submit face enroll, jpeg size:" << jpeg.size();
    QString errorString;
    if (!m_proxy->startEnroll(KAD_AUTH_TYPE_FACE, m_inErollFeatureName, extraInfo, errorString))
    {
        // 会话建立失败(如特征数达上限、无设备):保留预览可重试
        KLOG_WARNING(qLcAuthentication) << "start enroll face failed," << errorString;
        m_enrollTips->setText(QString(tr("Start enroll failed,%1")).arg(errorString));
        m_captureButton->setEnabled(true);
        return;
    }

    m_inEnroll = true;
    m_enrollTips->setText(tr("Processing..."));
}

void FacePage::updateEnrollStatus(const QString& iid, bool isComplete,
                                  int progress, const QString& message)
{
    KLOG_DEBUG(qLcAuthentication) << "update enroll status:"
                                  << "iid" << iid << ","
                                  << "complete" << isComplete << ","
                                  << "progress" << progress << ","
                                  << "message" << message;

    if (!message.isEmpty())
    {
        m_enrollTips->setText(message);
    }

    if (isComplete)
    {
        finishEnroll(!iid.isEmpty(), message, iid);
    }
}

void FacePage::enrollCancel()
{
    KLOG_INFO(qLcAuthentication) << "cancel enroll face";

    if (m_inEnroll)
    {
        m_proxy->stopEnroll();
    }
    m_inEnroll = false;
    m_capturing = false;
    m_idleTimer->stop();
    stopCamera();
    m_inErollFeatureName.clear();

    m_stackedWidget->setCurrentIndex(PAGE_INDEX_FEATURE_MANAGER);
}

void FacePage::finishEnroll(bool isSuccess, const QString& message, const QString& iid)
{
    KLOG_INFO(qLcAuthentication) << "enroll face complete iid:" << iid
                                 << "message:" << message;

    m_inEnroll = false;
    if (isSuccess)
    {
        m_idleTimer->stop();
        stopCamera();
        m_featureManager->refreshFeature();
        QString text = QString(tr("The biometric features were successfully recorded. The feature name is:%1")).arg(m_inErollFeatureName);
        KiranMessageBox::message(this, tr("Tips"), text, KiranMessageBox::Ok);
        m_inErollFeatureName.clear();
        m_stackedWidget->setCurrentIndex(PAGE_INDEX_FEATURE_MANAGER);
    }
    else
    {
        // 失败:保留预览,提示原因,可直接重试(US1 场景 4)
        m_enrollTips->setText(QString(tr("Failed to record(%1), please try again")).arg(message));
        m_captureButton->setEnabled(true);
        m_idleTimer->start();
    }
}

void FacePage::onIdleTimeout()
{
    KLOG_INFO(qLcAuthentication) << "enroll preview idle timeout, cancel";

    if (m_inEnroll)
    {
        m_proxy->stopEnroll();
    }
    m_inEnroll = false;
    m_capturing = false;
    stopCamera();
    m_inErollFeatureName.clear();

    m_stackedWidget->setCurrentIndex(PAGE_INDEX_FEATURE_MANAGER);
}
