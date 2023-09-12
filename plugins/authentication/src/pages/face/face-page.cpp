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
#include "utils/kiran-auth-dbus-proxy.h"
#include "widgets/auth-setting-container.h"
#include "widgets/auth-setting-item.h"
#include "widgets/general-bio-page.h"
#include "widgets/image-enroll-progressbar.h"

#include <kiran-message-box.h>
#include <qt5-log-i.h>
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QStackedWidget>

#define PAGE_INDEX_FEATURE_MANAGER 0
#define PAGE_INDEX_FEATURE_ENROLL 1

FacePage::FacePage(KiranAuthDBusProxy* proxy, QWidget* parent)
    : QWidget(parent),
      m_proxy(proxy)
{
    initUI();
    connect(m_proxy, &KiranAuthDBusProxy::EnrollStatus, this, &FacePage::onEnrollStatusNotify);
}

FacePage::~FacePage()
{
    if (m_inEnroll)
    {
        m_proxy->stopEnroll();
    }
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
    connect(m_featureManager, &GeneralBioPage::enrollFeature, this, &FacePage::onEnrollFeatureClicked);

    return m_featureManager;
}

QWidget* FacePage::initFeatureEnrollPage()
{
    auto featureEnrollWidget = new QWidget();
    auto featureEnrollLayout = new QBoxLayout(QBoxLayout::TopToBottom, featureEnrollWidget);
    featureEnrollLayout->setSpacing(0);
    featureEnrollLayout->setContentsMargins(0, 0, 0, 16);

    m_enrollProgress = new ImageEnrollProgressBar(featureEnrollWidget);
    m_enrollProgress->registerPercentImage(0, ":/kcp-authentication/images/face-0.svg");
    m_enrollProgress->setFixedSize(272, 272);
    m_enrollProgress->setProgress(50);
    featureEnrollLayout->addWidget(m_enrollProgress, 0, Qt::AlignHCenter);

    featureEnrollLayout->addSpacerItem(new QSpacerItem(10, 16, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_enrollTips = new QLabel;
    m_enrollTips->setAlignment(Qt::AlignHCenter);
    featureEnrollLayout->addWidget(m_enrollTips);

    featureEnrollLayout->addStretch();

    auto buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    buttonLayout->addStretch();

    auto cancelButton = new QPushButton(this);
    cancelButton->setFixedSize(110, 40);
    cancelButton->setText(tr("Cancel"));
    buttonLayout->addWidget(cancelButton);
    connect(cancelButton, &QPushButton::clicked, this, &FacePage::onEnrollCancelClicked);

    buttonLayout->addStretch();
    featureEnrollLayout->addLayout(buttonLayout);

    return featureEnrollWidget;
}

void FacePage::onEnrollStatusNotify(const QString& iid, bool isComplete,
                                    int progress, const QString& message)
{
    KLOG_DEBUG() << "enroll status notify:" << iid << isComplete << progress << message;
    m_enrollProgress->setProgress(progress);
    if (!message.isEmpty())
    {
        m_enrollTips->setText(message);
    }

    if (isComplete)
    {
        onEnrollComplete(!iid.isEmpty(), message, iid);
    }
}

void FacePage::onEnrollFeatureClicked()
{
    QString featureName = m_featureManager->autoGenerateFeatureName();

    m_enrollProgress->setProgress(0);
    m_enrollTips->setText("");

    m_inEnroll = true;
    m_inErollFeatureName = featureName;

    QString errorString;
    if (!m_proxy->startEnroll(KAD_AUTH_TYPE_FACE, featureName, QString(), errorString))
    {
        m_inEnroll = false;
        m_inErollFeatureName.clear();
        QString text = QString(tr("Start enroll failed,%1").arg(errorString));
        KiranMessageBox::message(this, tr("Error"), text, KiranMessageBox::Ok);
        return;
    }

    m_stackedWidget->setCurrentIndex(PAGE_INDEX_FEATURE_ENROLL);
}

void FacePage::onEnrollCancelClicked()
{
    m_proxy->stopEnroll();
    m_inEnroll = false;
    m_inErollFeatureName.clear();

    m_stackedWidget->setCurrentIndex(PAGE_INDEX_FEATURE_MANAGER);
}

void FacePage::onEnrollComplete(bool isSuccess, const QString& message, const QString& iid)
{
    KLOG_DEBUG() << "enroll complete iid:" << iid << "message:" << message;
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
    m_stackedWidget->setCurrentIndex(PAGE_INDEX_FEATURE_MANAGER);
}