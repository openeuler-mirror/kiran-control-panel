/**
 * Copyright (c) 2023 ~ 2024 KylinSec Co., Ltd.
 * ks-sc is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yinhongchang <yinhongchang@kylinsec.com.cn>
 */

#include "mimetype-apppage.h"
#include <qt5-log-i.h>
#include <QBoxLayout>
#include <QListWidget>
#include "app-manager.h"
#include "kiran-setting-container.h"

MimeTypeAppPage::MimeTypeAppPage(EnumMimeType enumMimeType, QWidget* parent)
    : m_enumMimeType(enumMimeType), QWidget(parent), m_appManager(new AppManager), m_application()
{
    initUI();
}

MimeTypeAppPage::~MimeTypeAppPage()
{
    delete m_appManager;
    m_appManager = nullptr;
}

void MimeTypeAppPage::initUI()
{
    auto mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(0);

    auto authTypeWidget = new QWidget(this);
    mainLayout->addWidget(authTypeWidget);

    auto authTypeLayout = new QBoxLayout(QBoxLayout::TopToBottom, authTypeWidget);
    authTypeLayout->setSpacing(8);
    authTypeLayout->setContentsMargins(10, 0, 10, 0);

    authTypeLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));

    auto container = new SettingsContainer(this);
    authTypeLayout->addWidget(container);

    authTypeLayout->addStretch();

    QVector<XdgDesktopFilePtr> m_appInfos = m_appManager->getApplications(m_enumMimeType);
    for (int index = 0; index < m_appInfos.size(); index++)
    {
        // 获取desktop的中文名和图标
        XdgDesktopFile* deskInfo = m_appInfos[index].data();

        auto settingsItem = new KiranSettingItem(this);
        settingsItem->setUserData(QFileInfo(deskInfo->fileName()).fileName());

        settingsItem->setClickable(true);
        settingsItem->setLeftButtonVisible(true, deskInfo->icon(), false);

        if (index == 0)
        {
            settingsItem->setRightButtonVisible(true, QIcon(":/kcp-defaultapp-images/chosen.png"), false);
            m_beforeItem = settingsItem;
        }
        else
        {
            settingsItem->setRightButtonVisible(false, QIcon(":/kcp-defaultapp-images/chosen.png"), false);
        }
        settingsItem->setText(deskInfo->name());
        container->addKiranSettingItem(settingsItem);

        connect(settingsItem, &KiranSettingItem::clicked, this, &MimeTypeAppPage::onClickedSlot);

        m_application[QFileInfo(deskInfo->fileName()).fileName()] = m_appInfos[index];
    }
}

// 切换选项
void MimeTypeAppPage::onClickedSlot()
{
    KiranSettingItem* settingsItem = qobject_cast<KiranSettingItem*>(sender());

    m_beforeItem->setRightButtonVisible(false, QIcon(":/kcp-defaultapp-images/chosen.png"), false);
    m_beforeItem = settingsItem;

    settingsItem->setRightButtonVisible(true, QIcon(":/kcp-defaultapp-images/chosen.png"), false);
    m_appManager->slotTextChanged(m_enumMimeType, m_application[settingsItem->getUserData().toString()].data());

}
