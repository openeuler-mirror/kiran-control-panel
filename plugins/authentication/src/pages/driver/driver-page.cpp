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
#include "utils/kiran-auth-dbus-proxy.h"
#include "widgets/auth-setting-container.h"
#include "widgets/auth-setting-item.h"

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

    m_combobox = new QComboBox(this);
    layout->addWidget(m_combobox);
    connect(m_combobox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DriverPage::onComboxBoxCurrentIndexChanged);

    layout->addSpacerItem(new QSpacerItem(10, 16, QSizePolicy::Minimum, QSizePolicy::Fixed));

    auto labelDriverList = new QLabel(tr("driver list"));
    layout->addWidget(labelDriverList);

    layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));

    m_container = new AuthSettingContainer(this);
    layout->addWidget(m_container, 1);

    layout->addStretch();

    m_combobox->addItem(tr("Fingerprint"), KAD_AUTH_TYPE_FINGERPRINT);
    m_combobox->addItem(tr("FingerVein"), KAD_AUTH_TYPE_FINGERVEIN);
}

void DriverPage::refreshAllDriver()
{
    m_container->clear();
    auto drivers = m_proxy->getDriversByType(m_authType);

    for (auto driver : drivers)
    {
        auto name = driver.name;
        auto enable = driver.enabled;

        auto featureItem = new AuthSettingItem;
        featureItem->setText(name);
        featureItem->setSwitcherVisible(true);
        featureItem->setSwitcherChecked(enable);

        m_container->addFeatureItem(featureItem);
        connect(featureItem, &AuthSettingItem::switchButtonToggled,
                this, std::bind(&DriverPage::onDriverSwticherToggled, this, name, std::placeholders::_2));
    }
}

void DriverPage::onComboxBoxCurrentIndexChanged(int index)
{
    m_authType = (KADAuthType)m_combobox->itemData(index).toInt();
    refreshAllDriver();
}

void DriverPage::onDriverSwticherToggled(const QString& driverName, bool checked)
{
    m_proxy->setDriverEnalbe(driverName,checked);

    // 不应在槽处理里删除所有的驱动控件，延后，等到下一个事件循环再执行刷新
    QTimer::singleShot(0,[this](){
        refreshAllDriver();
    });
}