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

#include "fingerprint-enroll-dialog.h"
#include <kiran-system-daemon/accounts-i.h>
#include "ksd_accounts_proxy.h"
#include "ksd_accounts_user_proxy.h"
#include "ksd_biometrics_proxy.h"
#include "ui_fingerprint-enroll-dialog.h"

#include <kiran-message-box.h>
#include <qt5-log-i.h>

FingerprintEnrollDialog::FingerprintEnrollDialog(QWidget *parent)
    : KiranTitlebarWindow(parent),
      ui(new Ui::FingerprintEnrollDialog),
      m_interface(new KSDBiometricsProxy("com.kylinsec.Kiran.SystemDaemon.Biometrics",
                                         "/com/kylinsec/Kiran/SystemDaemon/Biometrics",
                                         QDBusConnection::systemBus(), this))
{
    ui->setupUi(getWindowContentWidget());
    init();
    m_worker.startFingerprintEnroll();
}

FingerprintEnrollDialog::~FingerprintEnrollDialog()
{
    m_worker.stopFingerprintEnroll();
    delete ui;
}

void FingerprintEnrollDialog::init()
{
    ///设置窗口模态
    setWindowModality(Qt::ApplicationModal);
    setWindowFlag(Qt::Dialog);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setTitle(tr("Finger Enroll"));
    setFixedSize(444, 555);
    setButtonHints(KiranTitlebarWindow::TitlebarCloseButtonHint);

    connect(&m_worker, &FingerprintInputWorker::sigShowStatus, this, &FingerprintEnrollDialog::slotShowStatus);
    connect(&m_worker, &FingerprintInputWorker::sigEnrollComplete, this, &FingerprintEnrollDialog::slotEnrollComplete);
    connect(&m_worker, &FingerprintInputWorker::sigEnrollError, this, &FingerprintEnrollDialog::slotEnrollError);
    connect(ui->btn_save, &QPushButton::clicked, [this]() {
        m_isSave = true;
        this->close();
    });
    connect(ui->btn_cancel, &QPushButton::clicked, [this]() {
        m_isSave = false;
        this->close();
    });
}

void FingerprintEnrollDialog::closeEvent(QCloseEvent *event)
{
    if (!m_isSave && !m_fingerDataID.isEmpty())
    {
        auto deleteEnrolledReply = m_interface->DeleteEnrolledFinger(m_fingerDataID);
        deleteEnrolledReply.waitForFinished();
        if (!deleteEnrolledReply.isError())
        {
            KLOG_ERROR() << "delete enrolled finger failed!" << deleteEnrolledReply.error();
        }
        m_fingerDataID.clear();
    }
    emit sigClose();
    QWidget::closeEvent(event);
}

void FingerprintEnrollDialog::setTips(FingerprintEnrollDialog::TipType type,
                                      const QString &tip)
{
    QString colorText = QString("<font color=%1>%2</font>")
                            .arg(type == TIP_TYPE_INFO ? "white" : "red")
                            .arg(tip);
    ui->label_msg->setText(colorText);
}

void FingerprintEnrollDialog::slotShowStatus(unsigned int progress, const QString &msg)
{
    setProgress(progress);
    setTips(TIP_TYPE_INFO, msg);
}

void FingerprintEnrollDialog::slotEnrollComplete(bool isSuccess, const QString &msg, const QString &id)
{
    if (isSuccess)
    {
        // 查找该模板是否绑定用户
        bool alreadyBinding = false;
        QString bindingUserName;

        KSDAccountsProxy accountsProxy(ACCOUNTS_DBUS_NAME,
                                       ACCOUNTS_OBJECT_PATH,
                                       QDBusConnection::systemBus());
        auto findUserReply = accountsProxy.FindUserByAuthData(ACCOUNTS_AUTH_MODE_FINGERPRINT, id);
        findUserReply.waitForFinished();
        if (findUserReply.isError())
        {
            KLOG_ERROR() << "find user error:" << findUserReply.error();
        }
        else
        {
            alreadyBinding = true;
            QString userObjectPath = findUserReply.value().path();
            KSDAccountsUserProxy accountsUserProxy(ACCOUNTS_DBUS_NAME,
                                                   userObjectPath,
                                                   QDBusConnection::systemBus());
            bindingUserName = accountsUserProxy.user_name();
        }

        if (alreadyBinding)
        {
            setProgress(0);
            QString errmsg = QString(tr("This fingerprint is bound to the user(%1)")).arg(bindingUserName);
            auto res = KiranMessageBox::message(this,
                                                tr("Info"),
                                                errmsg,
                                                KiranMessageBox::Cancel | KiranMessageBox::Retry);
            if (res == KiranMessageBox::Cancel)
            {
                this->close();
                return;
            }
            else
            {
                m_worker.startFingerprintEnroll();
                return;
            }
        }
        else
        {
            setProgress(100);
            setTips(TIP_TYPE_INFO, msg);
            m_fingerDataID = id;
        }
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

void FingerprintEnrollDialog::slotEnrollError(const QString &errMsg)
{
    setProgress(0);
    setTips(TIP_TYPE_ERROR, errMsg);
}

void FingerprintEnrollDialog::setProgress(unsigned int value)
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

QString FingerprintEnrollDialog::getFingerDataID()
{
    return m_fingerDataID;
}

#include <QResizeEvent>

void FingerprintEnrollDialog::resizeEvent(QResizeEvent *event)
{
    KLOG_INFO() << "fingerprint size:" << event->size();
    QWidget::resizeEvent(event);
}
