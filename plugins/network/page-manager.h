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
#pragma once
#include <QObject>
#include <QWidget>
#include <QMap>

namespace Kiran
{
namespace Network
{
enum PageType
{
    PAGE_ETHERNET = 0,
    PAGE_WIRELESS,
    PAGE_VPN,
    PAGE_DETAILS,
    PAGE_LAST
};
class Page : public QWidget
{
    Q_OBJECT
public:
    Page() = delete;
    explicit Page(PageType type, QWidget* parent = nullptr);
    virtual ~Page() = default;

    virtual void reset() = 0;
    PageType type() { return m_type; };

private:
    PageType m_type;
};

struct PageInfo
{
    QString name;
    QString key;
    QString icon;
};
class DeviceManager;
#define PM_INSTANCE PageManager::instance()
class PageManager : public QObject
{
    Q_OBJECT
public:
    ~PageManager() = default;

    static PageManager* instance() { return instance_; };
    static void global_init(DeviceManager* deviceManager);
    static void global_deinit() { delete instance_; };

    Page* create(PageType type);
    bool isAvaliable(PageType type);
    PageInfo description(PageType type);

signals:
    void pageAvailableChanged();
    // 请求跳转编辑Ethernet/网络连接设置界面
    void requestEditConnectionSettings(const QString& devicePath, const QString& connectionPath);
    // 请求跳转Etherernet/有线网络设备界面
    void requestWiredDevicePage();

private:
    PageManager();
    void init(DeviceManager* deviceManager);

private slots:
    void processManagedDeviceChanged();

private:
    static PageManager* instance_;
    DeviceManager* m_deviceManager;
    QMap<PageType, bool> m_pageAvailable;
};
}  // namespace Network
}  // namespace Kiran
