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
#include "widgets/finger-enroll-progressbar.h"
#include "identification-rename-dialog.h"

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

FingerPage::FingerPage(KiranAuthDBusProxy* proxy, FingerAuthType type, QWidget* parent)
    : QWidget(parent),
      m_type(type),
      m_authType(type == FINGER_TYPE_FINGER_PRINT ? KAD_AUTH_TYPE_FINGERPRINT : KAD_AUTH_TYPE_FINGERVEIN),
      m_proxy(proxy),
      m_inEnroll(false)
{
    initUI();

    connect(m_proxy, &KiranAuthDBusProxy::EnrollStatus, this, &FingerPage::onEnrollStatusNotify);

    m_stackedWidget->setCurrentIndex(FINGER_PAGE_INDEX_MANAGER);

    refreshAllFeature();
    refreshDeviceComboBox();
}

FingerPage::~FingerPage()
{
    if( m_inEnroll )
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

void FingerPage::refreshAllFeature()
{
    m_settingContainer->clear();
    m_featureNameSet.clear();

    auto identifications = m_proxy->getUserIdentifications(m_authType);
    FINGER_DEBUG() << "reload identifications"
                   << ",count:" << identifications.count();

    for (auto iter : identifications)
    {
        auto featureItem = new AuthSettingItem;
        featureItem->setUserData(iter.IID);
        featureItem->setText(iter.name);
        featureItem->setLeftButtonVisible(true, ":/kcp-authentication/images/rename.svg");
        featureItem->setRightButtonVisible(true, ":/kcp-authentication/images/trash.svg");

        m_settingContainer->addFeatureItem(featureItem);
        m_featureNameSet << iter.name;

        connect(featureItem, &AuthSettingItem::leftButtonClicked, this, &FingerPage::onRenameIdentification);
        connect(featureItem, &AuthSettingItem::rightButtonClicked, this, &FingerPage::onDeleteIdentification);
    }
}

// 若未指定默认设备时，默认使用第一个设备作为默认设备
void FingerPage::refreshDeviceComboBox()
{
    QSignalBlocker blocker(m_defaultDeviceCombobox);
    m_defaultDeviceCombobox->clear();

    auto devices = m_proxy->getDevicesByType(m_authType);
    FINGER_DEBUG() << "reload" << m_authType << "devices"
                   << ",count:" << devices.count();

    auto defaultDevice = m_proxy->getDefaultDeviceID(m_authType);
    FINGER_DEBUG() << m_authType << "default device id:" << defaultDevice;

    for (auto device : devices)
    {
        m_defaultDeviceCombobox->addItem(device.name, device.id);
        if (!defaultDevice.isEmpty() && defaultDevice == device.id)
        {
            m_defaultDeviceCombobox->setCurrentIndex(m_defaultDeviceCombobox->count() - 1);
        }
    }
}

QWidget* FingerPage::initFeatureManager()
{
    auto featureManagerWidget = new QWidget(this);
    auto featureManagerLayout = new QBoxLayout(QBoxLayout::TopToBottom, featureManagerWidget);
    featureManagerLayout->setSpacing(0);
    featureManagerLayout->setContentsMargins(0, 0, 0, 0);

    auto labelDefaultDevice = new QLabel(tr("default fingerprint device"));
    featureManagerLayout->addWidget(labelDefaultDevice);

    featureManagerLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_defaultDeviceCombobox = new QComboBox(this);
    featureManagerLayout->addWidget(m_defaultDeviceCombobox);
    connect(m_defaultDeviceCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FingerPage::onDefaultDeviceCurrentIdxChanged);

    featureManagerLayout->addSpacerItem(new QSpacerItem(10, 16, QSizePolicy::Minimum, QSizePolicy::Fixed));

    auto labelFingerList = new QLabel(tr("fingerprint list"));
    featureManagerLayout->addWidget(labelFingerList);

    featureManagerLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_settingContainer = new AuthSettingContainer(this);
    featureManagerLayout->addWidget(m_settingContainer, 1);

    featureManagerLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));

    auto addButton = new QPushButton(this);
    featureManagerLayout->addWidget(addButton);
    addButton->setIcon(QPixmap(":/kcp-keyboard/images/addition.svg"));
    Kiran::StylePropertyHelper::setButtonType(addButton, Kiran::BUTTON_Default);
    connect(addButton, &QPushButton::clicked, this, &FingerPage::onAddIdentificationClicked);

    featureManagerLayout->addStretch();
    return featureManagerWidget;
}

QWidget* FingerPage::initFeatureEnroll()
{
    auto featureEnrollWidget = new QWidget();
    auto featureEnrollLayout = new QBoxLayout(QBoxLayout::TopToBottom, featureEnrollWidget);
    featureEnrollLayout->setSpacing(0);
    featureEnrollLayout->setContentsMargins(0, 0, 0, 16);

    m_enrollProgress = new FingerEnrollProgressBar(featureEnrollWidget);
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

QString FingerPage::autoGenerateFeatureName()
{
    QRandomGenerator randomGenerator;
    QString prefix = m_authType == KAD_AUTH_TYPE_FINGERPRINT ? tr("Fingerprint") : tr("FingerVein");

    for (int i = 0; i <= 10; ++i)
    {
        auto featureNumber = randomGenerator.bounded(1, MAX_FEATURE_NUMBER);
        auto temp = QString("%1 %2").arg(prefix).arg(featureNumber);

        if( !m_featureNameSet.contains(temp) )
        {
            return temp;
        }
    }

    return QString();
}

void FingerPage::onDefaultDeviceCurrentIdxChanged(int idx)
{
    QVariant userData = m_defaultDeviceCombobox->itemData(idx);
    m_proxy->setDefaultDeviceID(m_authType, userData.toString());
}

void FingerPage::onRenameIdentification(const QVariant& userData)
{
    QString iid = userData.toString();
    QScopedPointer<IdentificationRenameDialog> renameDialog(new IdentificationRenameDialog(iid,m_proxy,this));
    if( renameDialog->exec() )
    {
        refreshAllFeature();
    }
}

void FingerPage::onDeleteIdentification(const QVariant& userData)
{
    m_proxy->deleteIdentification(userData.toString());
    refreshAllFeature();
}

void FingerPage::onAddIdentificationClicked()
{
    QString featureName = autoGenerateFeatureName();

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
        refreshAllFeature();
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
