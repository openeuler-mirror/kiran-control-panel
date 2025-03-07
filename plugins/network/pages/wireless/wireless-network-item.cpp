/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
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
#include "wireless-network-item.h"
#include <kiran-message-box.h>
#include <qt5-log-i.h>
#include <NetworkManagerQt/Manager>
#include <QAction>
#include <QMenu>
#include <QMouseEvent>
#include <QProcess>
#include <tuple>
#include "logging-category.h"
#include "wireless-dialog.h"

using namespace NetworkManager;

namespace Kiran
{
namespace Network
{
WirelessNetworkItem::WirelessNetworkItem(WirelessDevice::Ptr& device,
                                         WirelessNetworkManager* networkManager,
                                         WirelessNetworkInfo networkInfo,
                                         QWidget* parent) : ConnectionItem(parent),
                                                            m_device(device),
                                                            m_networkManager(networkManager)
{
    init(networkInfo);
}

WirelessNetworkItem::~WirelessNetworkItem()
{
}

void WirelessNetworkItem::init(WirelessNetworkInfo networkInfo)
{
    m_ssid = networkInfo.ssid;
    m_referencePointPath = networkInfo.referencePointPath;
    m_signalStrength = networkInfo.signalStreagth;

    if (m_signalStrength >= 0)
    {
        m_isSecure = m_networkManager->networkBestSecurityType(m_ssid) != SECURITY_TYPE_NONE;
    }

    auto iconName = getNetworkSignalIcon(m_signalStrength, m_isSecure);

    setTypeIcon(iconName);
    setName(m_ssid);
    setEditable(false);
    setDisconnectable(false);
    setMoreOptionsEnable(false);

    auto moreMenu = new QMenu(this);
    m_ignoreAction = moreMenu->addAction(tr("Ignore"), this, SLOT(ignoreNetwork()));
    m_advancedSettingsAction = moreMenu->addAction(tr("Advanced Settings"), this, SLOT(showAdvancedSettings()));
    moreMenu->setStyleSheet("QPushButton::menu-indicator{image:none}");
    setMoreOptions(moreMenu);

    connect(this, &ConnectionItem::requestDisconnect, this, &WirelessNetworkItem::deactivateNetwork);
    connect(this, &ConnectionItem::requestActive, this, &WirelessNetworkItem::tryActivateNetwork);
    connect(m_networkManager, &WirelessNetworkManager::activeNetworkChanged, this, &WirelessNetworkItem::updateActivatedState);
    connect(m_networkManager, &WirelessNetworkManager::stateChanged, this, &WirelessNetworkItem::updateActivatedState);

    updateActivatedState();
}

QString WirelessNetworkItem::getNetworkSignalIcon(int signalPercent, bool isSecure)
{
    if (signalPercent < 0)
    {
        return "ksvg-kcp-wired";
    }

    int signalLevel = signalPercent / 25.0 + 1;
    if (signalLevel > 4)
        signalLevel = 4;

    QString iconName = QString("krsvg-kcp-wireless-%1").arg(signalLevel);
    if (isSecure)
    {
        iconName.append("-security");
    }

    return iconName;
}

void WirelessNetworkItem::activateHiddenNetwork()
{
    // 询问无线网络SSID
    QString ssid;
    if (!WirelessDialog::getHiddenNetworkSsid(parentWidget(), ssid))
    {
        return;
    }

    // 询问无线网络密码
    QString passwd;
    if (!WirelessDialog::getNetworkPasswd(parentWidget(), ssid, passwd))
    {
        return;
    }

    // 走PSK
    auto securityType = SECURITY_TYPE_WPA_AND_WPA2_PERSON;
    auto pendingReply = m_networkManager->addAndActivateHiddenNetwork(ssid, passwd, securityType);
    checkOpeartionResult(OPERTION_ACTIVATE, ssid, pendingReply);
}

void WirelessNetworkItem::updateActivatedState()
{
    QString activatedNetwork = m_networkManager->activatedNetowrk();
    bool isActivated = (activatedNetwork == m_ssid);
    bool isLoading = false;
    if (isActivated)
    {
        auto deviceState = m_networkManager->state();
        isLoading = deviceState > Device::State::Disconnected && deviceState < Device::State::Activated;
    }

    updateStatus(isActivated, isLoading);
    setDisconnectable(isActivated);
    setMoreOptionsEnable(isActivated);
}

void WirelessNetworkItem::ignoreNetwork()
{
    m_networkManager->removeNetworkConnection(m_ssid);
}

void WirelessNetworkItem::showAdvancedSettings()
{
    if (m_networkManager->activatedNetowrk() != m_ssid)
    {
        return;
    }

    auto activeConnection = m_device->activeConnection();
    auto connection = activeConnection->connection();
    auto uuid = connection->uuid();

    QProcess::startDetached("nm-connection-editor", {"-e", uuid});
}

void WirelessNetworkItem::deactivateNetwork()
{
    if (m_networkManager->activatedNetowrk() == m_ssid)
    {
        auto pendingReply = m_networkManager->deactivateConnection();
        checkOpeartionResult(OPERTION_DEACTIVATE, m_ssid, pendingReply);
    }
}

void WirelessNetworkItem::tryActivateNetwork()
{
    // AP为空，隐藏网络
    if (m_referencePointPath.isEmpty())
    {
        activateHiddenNetwork();
        return;
    }

    // 已激活，忽略
    if (m_networkManager->activatedNetowrk() == m_ssid)
    {
        return;
    }

    // 可直接连接（存在已有配置或无需认证），直接激活
    if (m_networkManager->checkNetworkCanDirectConn(m_ssid))
    {
        KLOG_INFO(qLcNetwork) << "try activate network" << m_ssid << ",can direct connect";
        auto pendingReply = m_networkManager->activateNetowrk(m_ssid);
        checkOpeartionResult(OPERTION_ACTIVATE,m_ssid,pendingReply);
        return;
    }

    // 确认无线网络安全类型
    auto securityType = m_networkManager->networkBestSecurityType(m_ssid);

    // 生成配置，然后激活
    QString passwd;
    switch (securityType)
    {
    case SECURITY_TYPE_WPA_AND_WPA2_PERSON:
    case SECURITY_TYPE_WPA3_PERSON:
        if (!WirelessDialog::getNetworkPasswd(parentWidget(), m_ssid, passwd))
        {
            return;
        }
    case SECURITY_TYPE_NONE:
    {
        auto pendingReply = m_networkManager->addAndActivateNetwork(m_ssid, passwd);
        checkOpeartionResult(OPERTION_ACTIVATE,m_ssid,pendingReply);
        break;
    }
    default:
        break;
    }
}
}  // namespace Network
}  // namespace Kiran