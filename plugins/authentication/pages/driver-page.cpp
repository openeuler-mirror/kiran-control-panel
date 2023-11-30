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
#include "driver-page.h"
#include "logging-category.h"
#include "utils/auth-setting-container.h"
#include "utils/auth-setting-item.h"
#include "utils/kiran-auth-dbus-proxy.h"

#include <qt5-log-i.h>
#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QTimer>

DriverPage::DriverPage(KiranAuthDBusProxy* proxy, QWidget* parent)
    : QWidget(parent),
      m_proxy(proxy)
{
    initUI();
}

DriverPage::~DriverPage()
{
}

void DriverPage::initUI()
{
    auto layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(0);

    auto labelDeviceType = new QLabel(tr("device type"));
    layout->addWidget(labelDeviceType);

    layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_authTypeComboBox = new QComboBox(this);
    layout->addWidget(m_authTypeComboBox);
    connect(m_authTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DriverPage::currentAuthTypIndexChanged);

    layout->addSpacerItem(new QSpacerItem(10, 16, QSizePolicy::Minimum, QSizePolicy::Fixed));

    auto labelDriverList = new QLabel(tr("driver list"));
    layout->addWidget(labelDriverList);

    layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_driverContainer = new AuthSettingContainer(this);
    layout->addWidget(m_driverContainer, 1);

    layout->addStretch();

    QMap<KADAuthType, QString> authTypeMap = {
        {KAD_AUTH_TYPE_FINGERPRINT, tr("Fingerprint")},
        {KAD_AUTH_TYPE_FINGERVEIN, tr("Fingervein")},
        {KAD_AUTH_TYPE_IRIS, tr("iris")},
        {KAD_AUTH_TYPE_UKEY, tr("ukey")},
        {KAD_AUTH_TYPE_FACE, tr("face")}};
    for (auto iter = authTypeMap.begin(); iter != authTypeMap.end(); iter++)
    {
        auto authType = iter.key();
        auto desc = iter.value();
        m_authTypeComboBox->addItem(desc, authType);
    }
}

void DriverPage::refreshAllDriver()
{
    KLOG_DEBUG(qLcAuthentication) << "refresh driver for" << m_currentAuthType;
    m_driverContainer->clear();
    auto drivers = m_proxy->getDriversByType(m_currentAuthType);

    for (auto driver : drivers)
    {
        auto name = driver.name;
        auto enable = driver.enabled;

        auto featureItem = new AuthSettingItem;
        featureItem->setText(name);
        featureItem->setSwitcherVisible(true);
        featureItem->setSwitcherChecked(enable);

        m_driverContainer->addAuthSettingItem(featureItem);
        connect(featureItem, &AuthSettingItem::switchButtonToggled,
                this, std::bind(&DriverPage::driverSwticherToggled, this, name, std::placeholders::_2));
    }
    KLOG_DEBUG(qLcAuthentication) << "refreshd driver" << drivers.count()
                                  << "for" << m_currentAuthType;
}

void DriverPage::currentAuthTypIndexChanged(int index)
{
    m_currentAuthType = (KADAuthType)m_authTypeComboBox->itemData(index).toInt();
    KLOG_DEBUG(qLcAuthentication) << "driver current auth type changed:" << index << m_currentAuthType;

    refreshAllDriver();
}

void DriverPage::driverSwticherToggled(const QString& driverName, bool checked)
{
    m_proxy->setDriverEnalbe(driverName, checked);

    // 不应在槽处理里删除所有的驱动控件，延后，等到下一个事件循环再执行刷新
    QTimer::singleShot(0, [this]()
                       { refreshAllDriver(); });
}