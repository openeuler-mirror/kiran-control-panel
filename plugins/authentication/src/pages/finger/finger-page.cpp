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
#include "utils/kiran-auth-dbus-proxy.h"
#include "widgets/auth-setting-container.h"
#include "widgets/auth-setting-item.h"
#include "widgets/image-enroll-progressbar.h"
#include "widgets/general-bio-page.h"

#include <kiran-message-box.h>
#include <qt5-log-i.h>
#include <style-property.h>
#include <sys/types.h>
#include <unistd.h>
#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QRandomGenerator>
#include <QStackedWidget>
#include <QUuid>

#define MAX_FEATURE_NUMBER 1000
#define FINGER_DEBUG() KLOG_DEBUG() << "auth type:" << this->m_authType

enum FingerPageIndexEnum
{
    FINGER_PAGE_INDEX_MANAGER,
    FINGER_PAGE_INDEX_ENROLL
};

const std::list<std::tuple<uint, QString>> FingerProgressRangePixmapList = {
    {0, ":/kcp-authentication/images/finger-0.svg"},
    {25, ":/kcp-authentication/images/finger-25.svg"},
    {50, ":/kcp-authentication/images/finger-50.svg"},
    {75, ":/kcp-authentication/images/finger-75.svg"},
    {100, ":/kcp-authentication/images/finger-100.svg"}};

FingerPage::FingerPage(KiranAuthDBusProxy* proxy, FingerAuthType type, QWidget* parent)
    : QWidget(parent),
      m_authType(type == FINGER_TYPE_FINGER_PRINT ? KAD_AUTH_TYPE_FINGERPRINT : KAD_AUTH_TYPE_FINGERVEIN),
      m_proxy(proxy),
      m_inEnroll(false)
{
    initUI();

    connect(m_proxy, &KiranAuthDBusProxy::EnrollStatus, this, &FingerPage::onEnrollStatusNotify);
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
    m_featureManager = new GeneralBioPage(m_proxy,m_authType,this);
    auto desc = m_authType==KAD_AUTH_TYPE_FINGERPRINT?tr("fingerprint"):tr("fingervein");
    m_featureManager->setFeatureNamePrefix(desc);
    m_featureManager->setDefaultDeviceLabelDesc(QString(tr("Default %1 device")).arg(desc));
    m_featureManager->setDeviceFeatureListDesc(QString(tr("%1 list").arg(desc)));
    connect(m_featureManager, &GeneralBioPage::enrollFeature, this, &FingerPage::onAddIdentificationClicked);
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
    connect(cancelButton, &QPushButton::clicked, this, &FingerPage::onEnrollCancelClicked);

    buttonLayout->addStretch();
    featureEnrollLayout->addLayout(buttonLayout);

    return featureEnrollWidget;
}

void FingerPage::onAddIdentificationClicked()
{
    QString featureName = m_featureManager->autoGenerateFeatureName();

    m_enrollProgress->setProgress(0);
    m_enRollTips->setText("");

    m_inEnroll = true;
    m_inErollFeatureName = featureName;

    QString errorString;
    if (!m_proxy->startEnroll(m_authType, featureName, QString(), errorString))
    {
        m_inEnroll = false;
        m_inErollFeatureName.clear();
        QString text = QString(tr("Start enroll failed,%1").arg(errorString));
        KiranMessageBox::message(this, tr("Error"), text, KiranMessageBox::Ok);
        return;
    }

    m_stackedWidget->setCurrentIndex(FINGER_PAGE_INDEX_ENROLL);
}

void FingerPage::onEnrollCancelClicked()
{
    m_proxy->stopEnroll();
    m_inEnroll = false;
    m_inErollFeatureName.clear();

    m_stackedWidget->setCurrentIndex(FINGER_PAGE_INDEX_MANAGER);
}

void FingerPage::onEnrollStatusNotify(const QString& iid, bool isComplete,
                                      int progress, const QString& message)
{
    FINGER_DEBUG() << "enroll status notify:" << iid << isComplete << progress << message;
    m_enrollProgress->setProgress(progress);
    if (!message.isEmpty())
    {
        m_enRollTips->setText(message);
    }

    if (isComplete)
    {
        onEnrollComplete(!iid.isEmpty(), message, iid);
    }
}

void FingerPage::onEnrollComplete(bool isSuccess, const QString& message, const QString& iid)
{
    FINGER_DEBUG() << "enroll complete iid:" << iid << "message:" << message;
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
