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
#include "page-manager.h"
#include "details/details-page.h"
#include "device-manager.h"
#include "ethernet/ethernet-page.h"
#include "logging-category.h"
#include "vpn/vpn-page.h"
#include "wireless/wireless-page.h"

namespace Kiran
{
namespace Network
{
Page::Page(PageType type, QWidget* parent)
    : QWidget(parent),
      m_type(type)
{
}

PageManager* PageManager::instance_ = nullptr;
void PageManager::global_init(DeviceManager* deviceManager)
{
    // 初始化传入DeviceManager只是为了强调依赖初始化顺序
    if (instance_ == nullptr)
    {
        instance_ = new PageManager();
        instance_->init(deviceManager);
    }
}

Page* PageManager::create(PageType type)
{
    Page* page = nullptr;
    switch (type)
    {
    case PAGE_ETHERNET:
        page = new EthernetPage();
        break;
    case PAGE_WIRELESS:
        page = new WirelessPage();
        break;
    case PAGE_VPN:
        page = new VpnPage();
        break;
    case PAGE_DETAILS:
        page = new DetailsPage();
        break;
    default:
        KLOG_ERROR(qLcNetwork) << "can't support this type" << type;
        break;
    }
    return page;
}

bool PageManager::isAvaliable(PageType type)
{
    if (m_pageAvailable.contains(type))
    {
        return m_pageAvailable[type];
    }
    return false;
}

PageInfo PageManager::description(PageType type)
{
    static PageInfo desc[] = {
        {QT_TR_NOOP("Wired Network"), "wired-network", "ksvg-kcp-wired"},
        {QT_TR_NOOP("Wireless Network"), "wireless-network", "ksvg-kcp-wireless"},
        {QT_TR_NOOP("VPN"), "vpn","ksvg-kcp-vpn"},
        {QT_TR_NOOP("Details"), "details","ksvg-kcp-network-details"}};

    PageInfo info;
    if (type < 0 && type >= PAGE_LAST)
    {
        return info;
    }

    info = desc[type];
    info.name = tr(info.name.toStdString().c_str());
    return info;
}

PageManager::PageManager()
{
}

void PageManager::init(DeviceManager* deviceManager)
{
    m_deviceManager = deviceManager;
    connect(m_deviceManager, &DeviceManager::managedDeviceChanged,
            this, &PageManager::processManagedDeviceChanged);
    processManagedDeviceChanged();
}

void PageManager::processManagedDeviceChanged()
{
    auto ethernetAvailable = m_deviceManager->managedWiredDeivces().size();
    auto wirelessAvailable = m_deviceManager->managedWirelessDeivces().size();
    QMap<PageType, bool> pageAvailable;
    pageAvailable[PAGE_ETHERNET] = ethernetAvailable;
    pageAvailable[PAGE_WIRELESS] = wirelessAvailable;
    pageAvailable[PAGE_VPN] = true;
    pageAvailable[PAGE_DETAILS] = true;

    if (pageAvailable != m_pageAvailable)
    {
        m_pageAvailable = pageAvailable;
        emit pageAvailableChanged();
    }
}

}  // namespace Network
}  // namespace Kiran
