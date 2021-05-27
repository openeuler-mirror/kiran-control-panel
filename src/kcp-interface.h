//
// Created by lxh on 2021/5/25.
//

#ifndef KIRAN_CPANEL_ACCOUNT_SRC_KCP_INTERFACE_H_
#define KIRAN_CPANEL_ACCOUNT_SRC_KCP_INTERFACE_H_

#include <kiran-control-panel/kcp-plugin-interface.h>

class QTranslator;
class KcpInterface : public QObject,public KcpPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterface_iid)
    Q_INTERFACES(KcpPluginInterface)

public:
    ~KcpInterface()= default;;
    int init() override;
    void uninit() override;

    QWidget* getSubItemWidget(QString subItemName) override;
    bool haveUnsavedOptions() override;
    QStringList visibleSubItems() override;

private:
    QWidget* m_currentWidget = nullptr;
    QTranslator* m_translator = nullptr;
};

#endif  //KIRAN_CPANEL_ACCOUNT_SRC_KCP_INTERFACE_H_
