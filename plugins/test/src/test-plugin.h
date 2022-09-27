#pragma once

#include <QTimer>
#include "kcp-plugin-interface-v2.h"
#include "subitem.h"

class TestPlugin : public QObject, public KcpPluginInterfaceV2
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterfaceV2_iid)
    Q_INTERFACES(KcpPluginInterfaceV2)

public:
    ~TestPlugin() = default;

    int init(KcpInterface* interface) override;
    void uninit() override;
    QVector<KcpPluginSubItemPtr> getSubItems() override;

private slots:
    void handleTimerTimeout();

private:
    QTimer m_timer;
    KcpInterface* m_controlPanelInterface;
    QVector<KcpPluginSubItemPtr> m_subitems;
};