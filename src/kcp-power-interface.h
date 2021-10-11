//
// Created by lxh on 2021/5/21.
//

#ifndef KIRAN_POWER_MANAGER_SRC_KCP_POWER_INTERFACE_H_
#define KIRAN_POWER_MANAGER_SRC_KCP_POWER_INTERFACE_H_

#include <kcp-plugin-interface.h>
#include <QTranslator>

class KcpPowerInterface : public QObject,public KcpPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterface_iid)
    Q_INTERFACES(KcpPluginInterface)

public:
    ~KcpPowerInterface();

    int init() override;
    void uninit() override;
    QWidget* getSubItemWidget(QString id) override;
    bool haveUnsavedOptions() override;
    QStringList visibleSubItems() override;

private:
    QWidget* m_currentWidget;
    QTranslator* m_translator = nullptr;
};

#endif  //KIRAN_POWER_MANAGER_SRC_KCP_POWER_INTERFACE_H_
