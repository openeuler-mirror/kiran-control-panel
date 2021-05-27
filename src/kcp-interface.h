//
// Created by lxh on 2021/5/26.
//

#ifndef KIRAN_TIMEDATE_MANAGER_SRC_KCP_INTERFACE_H_
#define KIRAN_TIMEDATE_MANAGER_SRC_KCP_INTERFACE_H_

#include <kcp-plugin-interface.h>

class QTranslator;
class KcpInterface : public QObject, public KcpPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterface_iid)
    Q_INTERFACES(KcpPluginInterface)

private:
    ~KcpInterface() override;
    int init() override;
    void uninit() override;
    QWidget* getSubItemWidget(QString id) override;
    bool haveUnsavedOptions() override;
    QStringList visibleSubItems() override;

private:
    QWidget* m_currentWidget = nullptr;
    QTranslator* m_translator = nullptr;
};

#endif  //KIRAN_TIMEDATE_MANAGER_SRC_KCP_INTERFACE_H_
