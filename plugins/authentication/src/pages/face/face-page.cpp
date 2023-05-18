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

#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSpacerItem>

FacePage::FacePage(KiranAuthDBusProxy* proxy, QWidget* parent)
    : QWidget(parent),
      m_proxy(proxy)
{
    initUI();
}

FacePage::~FacePage()
{
}

void FacePage::initUI()
{
    auto mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(0);

    m_featureManager = new GeneralBioPage(m_proxy,KAD_AUTH_TYPE_FACE,this);
    m_featureManager->setDefaultDeviceLabelDesc(tr("Default face device"));
    m_featureManager->setDeviceFeatureListDesc(tr("face feature list"));
    connect(m_featureManager, &GeneralBioPage::enrollFeatureClicked, this, &FacePage::onEnrollFeatureClicked);

    mainLayout->addWidget(m_featureManager);
}

void FacePage::onEnrollFeatureClicked()
{
    
}