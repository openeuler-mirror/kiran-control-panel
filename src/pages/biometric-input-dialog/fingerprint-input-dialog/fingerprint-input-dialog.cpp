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
 
#include "fingerprint-input-dialog.h"
#include "biometrics-interface.h"
#include "ui_fingerprint-input-dialog.h"

#include <kiran-message-box.h>
#include <qt5-log-i.h>

FingerprintInputDialog::FingerprintInputDialog(QWidget *parent)
    : KiranTitlebarWindow(parent),
      ui(new Ui::FingerprintInputDialog),
      m_interface(new BiometricsInterface(QDBusConnection::systemBus(), this))
{
    ui->setupUi(getWindowContentWidget());
    init();
    m_worker.startFingerprintEnroll();
}

FingerprintInputDialog::~FingerprintInputDialog()
{
    m_worker.stopFingerprintEnroll();
    delete ui;
}

void FingerprintInputDialog::init()
{
    ///设置窗口模态
    setWindowModality(Qt::ApplicationModal);
    setWindowFlag(Qt::Dialog);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setTitle(tr("Finger Enroll"));
    setFixedSize(444, 555);
    setButtonHints(KiranTitlebarWindow::TitlebarCloseButtonHint);

    connect(&m_worker, &FingerprintInputWorker::sigShowStatus, this, &FingerprintInputDialog::slotShowStatus);
    connect(&m_worker, &FingerprintInputWorker::sigEnrollComplete, this, &FingerprintInputDialog::slotEnrollComplete);
    connect(&m_worker, &FingerprintInputWorker::sigEnrollError, this, &FingerprintInputDialog::slotEnrollError);
    connect(ui->btn_save, &QPushButton::clicked, [this]() {
        m_isSave = true;
        this->close();
    });
    connect(ui->btn_cancel, &QPushButton::clicked, [this]() {
        m_isSave = false;
        this->close();
    });
}

void FingerprintInputDialog::closeEvent(QCloseEvent *event)
{
    if (!m_isSave && !m_fingerDataID.isEmpty())
    {
        auto deleteEnrolledReply = m_interface->DeleteEnrolledFinger(m_fingerDataID);
        deleteEnrolledReply.waitForFinished();
        if(!deleteEnrolledReply.isError())
        {
            KLOG_ERROR() << "delete enrolled finger failed!" << deleteEnrolledReply.error();
        }
        m_fingerDataID.clear();
    }
    emit sigClose();
    QWidget::closeEvent(event);
}

void FingerprintInputDialog::setTips(FingerprintInputDialog::TipType type,
                                     const QString &tip)
{
    QString colorText = QString("<font color=%1>%2</font>")
                            .arg(type == TIP_TYPE_INFO ? "white" : "red")
                            .arg(tip);
    ui->label_msg->setText(colorText);
}

void FingerprintInputDialog::slotShowStatus(unsigned int progress, const QString &msg)
{
    setProgress(progress);
    setTips(TIP_TYPE_INFO, msg);
}

void FingerprintInputDialog::slotEnrollComplete(bool isSuccess, const QString &msg, const QString &id)
{
    if (isSuccess)
    {
        setProgress(100);
        setTips(TIP_TYPE_INFO, msg);
        m_fingerDataID = id;
    }
    else
    {
        setProgress(0);
        setTips(TIP_TYPE_ERROR, msg);
        auto res = KiranMessageBox::message(this, tr("Error"), msg, KiranMessageBox::Cancel | KiranMessageBox::Retry);
        switch (res)
        {
        case KiranMessageBox::Cancel:
            this->close();
            break;
        case KiranMessageBox::Retry:
            m_worker.startFingerprintEnroll();
            break;
        default:
            break;
        }
    }
}

void FingerprintInputDialog::slotEnrollError(const QString &errMsg)
{
    setProgress(0);
    setTips(TIP_TYPE_ERROR, errMsg);
}

void FingerprintInputDialog::setProgress(unsigned int value)
{
    ui->enrollProgress->setProgressValue(value);
    struct ProgressPixmapInfo
    {
        unsigned int value;
        const char *image;
    };
    static const ProgressPixmapInfo pixmapArray[] = {
        {100, ":/kcp-account-images/finger_100.svg"},
        {75, ":/kcp-account-images/finger_75.svg"},
        {50, ":/kcp-account-images/finger_50.svg"},
        {25, ":/kcp-account-images/finger_25.svg"},
        {0, ":/kcp-account-images/finger_0.svg"}};
    QString progressImage = ":/kcp-account-images/finger_0.svg";
    for (auto i : pixmapArray)
    {
        if (value >= i.value)
        {
            progressImage = i.image;
            break;
        }
    }
    KLOG_INFO() << "progressImage" << progressImage;
    ui->enrollProgress->updateCenterImage(progressImage);
}

QString FingerprintInputDialog::getFingerDataID()
{
    return m_fingerDataID;
}

#include <QResizeEvent>

void FingerprintInputDialog::resizeEvent(QResizeEvent *event)
{
    KLOG_INFO() << "fingerprint size:" << event->size();
    QWidget::resizeEvent(event);
}
