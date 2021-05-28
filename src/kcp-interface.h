/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef __KCP_INTERFACE_H__
#define __KCP_INTERFACE_H__

#include <kcp-plugin-interface.h>
#include "kiran-display-module-base.h"

class QTranslator;
class KcpInterface : public QObject, public KcpPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterface_iid)
    Q_INTERFACES(KcpPluginInterface)

public:
    KcpInterface();
    ~KcpInterface();
    int init() override;
    void uninit() override;
    QWidget *getSubItemWidget(QString id) override;
    bool haveUnsavedOptions() override;
    QStringList visibleSubItems() override;

private:
    QTranslator* m_translator = nullptr;
};

#endif  // __KCP_INTERFACE_H__
