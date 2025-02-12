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

#include "finger-page.h"
#include "auxiliary.h"
#include "logging-category.h"

#include "utils/auth-setting-container.h"
#include "utils/auth-setting-item.h"
#include "utils/general-bio-page.h"
#include "utils/image-enroll-progressbar.h"
#include "utils/kiran-auth-dbus-proxy.h"

#include <kiran-message-box.h>
#include <qt5-log-i.h>
#include <sys/types.h>
#include <unistd.h>
#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QUuid>

#define MAX_FEATURE_NUMBER 1000

enum FingerPageIndexEnum
{
    FINGER_PAGE_INDEX_MANAGER,
    FINGER_PAGE_INDEX_ENROLL
};

const std::list<std::tuple<uint, QString>> FingerProgressRangePixmapList = {
    std::make_tuple(0, "krsvg-finger-0.svg"),
    std::make_tuple(25, "krsvg-finger-25.svg"),
    std::make_tuple(50, "krsvg-finger-50.svg"),
    std::make_tuple(75, "krsvg-finger-75.svg"),
    std::make_tuple(100, "krsvg-finger-100.svg")};

FingerPage::FingerPage(KiranAuthDBusProxy* proxy, FingerAuthType type, QWidget* parent)
    : QWidget(parent),
      m_authType(type == FINGER_TYPE_FINGER_PRINT ? KAD_AUTH_TYPE_FINGERPRINT : KAD_AUTH_TYPE_FINGERVEIN),
      m_proxy(proxy),
      m_inEnroll(false)
{
    initUI();

    connect(m_proxy, &KiranAuthDBusProxy::EnrollStatus, this, &FingerPage::updateEnrollStatus);
    m_stackedWidget->setCurrentIndex(FINGER_PAGE_INDEX_MANAGER);
}

FingerPage::~FingerPage()
{
    if (m_inEnroll)
    {
        m_proxy->stopEnroll();
    }
}

void FingerPage::initUI()
{
    auto mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(0);

    m_stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(m_stackedWidget);

    m_stackedWidget->insertWidget(FINGER_PAGE_INDEX_MANAGER, initFeatureManager());
    m_stackedWidget->insertWidget(FINGER_PAGE_INDEX_ENROLL, initFeatureEnroll());
}

QWidget* FingerPage::initFeatureManager()
{
    m_featureManager = new GeneralBioPage(m_proxy, m_authType, this);
    auto desc = m_authType == KAD_AUTH_TYPE_FINGERPRINT ? tr("fingerprint") : tr("fingervein");
    m_featureManager->setFeatureNamePrefix(desc);
    m_featureManager->setDefaultDeviceLabelDesc(QString(tr("Default %1 device")).arg(desc));
    m_featureManager->setDeviceFeatureListDesc(QString(tr("%1 list").arg(desc)));
    connect(m_featureManager, &GeneralBioPage::enrollFeature, this, &FingerPage::startEnroll);
    return m_featureManager;
}

QWidget* FingerPage::initFeatureEnroll()
{
    auto featureEnrollWidget = new QWidget();
    auto featureEnrollLayout = new QBoxLayout(QBoxLayout::TopToBottom, featureEnrollWidget);
    featureEnrollLayout->setSpacing(0);
    featureEnrollLayout->setContentsMargins(0, 0, 0, 16);

    m_enrollProgress = new ImageEnrollProgressBar(featureEnrollWidget);
    m_enrollProgress->registerPercentImages(FingerProgressRangePixmapList);
    m_enrollProgress->setFixedSize(272, 272);
    m_enrollProgress->setProgress(50);
    featureEnrollLayout->addWidget(m_enrollProgress, 0, Qt::AlignHCenter);

    featureEnrollLayout->addSpacerItem(new QSpacerItem(10, 16, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_enRollTips = new QLabel;
    m_enRollTips->setAlignment(Qt::AlignHCenter);
    featureEnrollLayout->addWidget(m_enRollTips);

    featureEnrollLayout->addStretch();

    auto buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    buttonLayout->addStretch();

    auto cancelButton = new QPushButton(this);
    cancelButton->setFixedSize(110, 40);
    cancelButton->setText(tr("Cancel"));
    buttonLayout->addWidget(cancelButton);
    connect(cancelButton, &QPushButton::clicked, this, &FingerPage::stopEnroll);

    buttonLayout->addStretch();
    featureEnrollLayout->addLayout(buttonLayout);

    return featureEnrollWidget;
}

void FingerPage::startEnroll()
{
    KLOG_INFO(qLcAuthentication) << "start enroll finger" << m_authType;

    QString featureName = m_featureManager->autoGenerateFeatureName();

    m_enrollProgress->setProgress(0);
    m_enRollTips->setText("");

    m_inEnroll = true;
    m_inErollFeatureName = featureName;

    QString errorString;
    if (!m_proxy->startEnroll(m_authType, featureName, QString(), errorString))
    {
        KLOG_WARNING(qLcAuthentication) << "start enroll failed," << errorString;
        m_inEnroll = false;
        m_inErollFeatureName.clear();
        QString text = QString(tr("Start enroll failed,%1").arg(errorString));
        KiranMessageBox::message(this, tr("Error"), text, KiranMessageBox::Ok);
        return;
    }

    m_stackedWidget->setCurrentIndex(FINGER_PAGE_INDEX_ENROLL);
}

void FingerPage::stopEnroll()
{
    KLOG_INFO(qLcAuthentication) << "enroll cancel";

    m_proxy->stopEnroll();
    m_inEnroll = false;
    m_inErollFeatureName.clear();

    m_stackedWidget->setCurrentIndex(FINGER_PAGE_INDEX_MANAGER);
}

void FingerPage::updateEnrollStatus(const QString& iid, bool isComplete,
                                    int progress, const QString& message)
{
    KLOG_DEBUG(qLcAuthentication) << "update enroll status:"
                                  << "iid" << iid << ","
                                  << "complete" << isComplete << ","
                                  << "progress" << progress << ","
                                  << "message" << message;

    m_enrollProgress->setProgress(progress);
    if (!message.isEmpty())
    {
        m_enRollTips->setText(message);
    }

    if (isComplete)
    {
        finishEnroll(!iid.isEmpty(), message, iid);
    }
}

void FingerPage::finishEnroll(bool isSuccess, const QString& message, const QString& iid)
{
    KLOG_INFO(qLcAuthentication) << "enroll finger complete iid:" << iid
                                 << "message:" << message;

    if (isSuccess)
    {
        m_featureManager->refreshFeature();
        QString text = QString(tr("The biometric features were successfully recorded. The feature name is:%1")).arg(m_inErollFeatureName);
        KiranMessageBox::message(this, tr("Tips"), text, KiranMessageBox::Ok);
    }
    else
    {
        QString text = QString(tr("Failed to record biometrics(%1), Please try again").arg(message));
        KiranMessageBox::message(this, tr("Error"), text, KiranMessageBox::Ok);
    }

    m_inEnroll = false;
    m_inErollFeatureName.clear();
    m_stackedWidget->setCurrentIndex(FINGER_PAGE_INDEX_MANAGER);
}
