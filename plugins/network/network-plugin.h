/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinsec.com.cn>
 */
#pragma once
#include "panel-interface.h"
#include "plugin-interface-v2.h"
#include "plugin-subitem-interface.h"

namespace Kiran
{
namespace Network
{
class Plugin
    : public QObject,
      public KiranControlPanel::PluginInterfaceV2
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KiranControlPanel_PluginInterfaceV2_iid)
    Q_INTERFACES(KiranControlPanel::PluginInterfaceV2)
public:
    Plugin(QObject* parent = nullptr);
    ~Plugin();
    int init(KiranControlPanel::PanelInterface* interface) override;
    void uninit() override;
    QVector<KiranControlPanel::SubItemPtr> getSubItems() override;

private:
    KiranControlPanel::SubItemPtr m_subitem;
};

class Widget;
class SubItem : public QObject,
                public KiranControlPanel::PluginSubitemInterface
{
    Q_OBJECT
public:
    explicit SubItem(KiranControlPanel::PanelInterface* interface, QObject* parent = nullptr);
    ~SubItem();

public:
    QString getID() override { return "Network"; };
    QString getName() override { return ""; };
    QString getCategory() override { return "network"; };
    QString getDesc() override { return ""; };
    QString getIcon() override { return ""; };
    int getWeight() override { return 0; };
    bool haveUnSavedOptions() override { return false; };

    QWidget* createWidget() override;
    QVector<QPair<QString, QString>> getSearchKeys() override;
    bool jumpToSearchEntry(const QString& key) override;

private slots:
    void processPageAvailableChanged();

private:
    KiranControlPanel::PanelInterface* m_interface = nullptr;
    Kiran::Network::Widget* m_widget = nullptr;
    QVector<QPair<QString, QString>> m_searchKeys;
};
}  // namespace Network
}  // namespace Kiran