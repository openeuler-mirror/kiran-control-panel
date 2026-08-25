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
#pragma once
#include <QSet>
#include <QWidget>

class KiranAuthDBusProxy;
class GeneralBioPage;
class QLabel;
class QStackedWidget;
class QPushButton;
class QTimer;
class QCamera;
class QCameraImageCapture;
class QCameraViewfinder;
class QVideoProbe;

class FacePage : public QWidget
{
    Q_OBJECT
public:
    FacePage(KiranAuthDBusProxy* proxy, QWidget* parent = nullptr);
    ~FacePage();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void initUI();
    QWidget* initFeatureManagerPage();
    QWidget* initFeatureEnrollPage();
    void startCamera();
    void stopCamera();
    void submitEnroll(const QImage& image);

private slots:
    void updateEnrollStatus(const QString& iid, bool isComplete,
                              int progress, const QString& message);
    void startEnrollFeature();
    void onCaptureClicked();
    void onImageCaptured(int id, const QImage& preview);
    void onImageCaptureError(int id, int error, const QString& errorString);
    void enrollCancel();
    void finishEnroll(bool isSuccess, const QString& message, const QString& iid);
    void onIdleTimeout();

private:
    KiranAuthDBusProxy* m_proxy;
    QSet<QString> m_featureNameSet;
    QStackedWidget* m_stackedWidget;
    GeneralBioPage* m_featureManager;

    // 录入页:摄像头实时预览(FR-002)
    QCameraViewfinder* m_viewfinder;
    QLabel* m_previewMask;  // 黑遮罩:首帧到达前覆盖预览区,防止残留 UI
    QVideoProbe* m_videoProbe = nullptr;
    QCamera* m_camera = nullptr;
    QCameraImageCapture* m_imageCapture = nullptr;
    QLabel* m_enrollTips;
    QPushButton* m_captureButton;
    QTimer* m_idleTimer;  // 预览 2 分钟无操作自动取消(FR-017)

    bool m_inEnroll = false;   // 服务端录入会话进行中(拍照提交后)
    bool m_capturing = false;  // 采集中(点击拍照后、图像回调前)
    QString m_inErollFeatureName;
};
