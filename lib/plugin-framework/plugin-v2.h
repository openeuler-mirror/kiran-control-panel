#include "kcp-plugin-interface-v2.h"
#include "plugin.h"

#include <QReadWriteLock>

class PluginV2 : public Plugin, public KcpInterface
{
    Q_OBJECT
public:
    PluginV2(QObject* parent = nullptr);
    ~PluginV2();

    /// @brief 加载指定路径下的插件共享库
    /// @param path 插件共享库的路径
    /// @return 是否加载成功
    bool load(const QString& path) override;
    /// @brief 卸载
    void unload() override;
    /// @brief 获取插件的功能项
    /// @return 功能项列表
    QVector<KcpPluginSubItemPtr> getSubItems() override;

    /// @brief 提供给插件实现回调的接口，插件通知该插件包装类向外发出信号
    /// @param subItemID 功能项ID
    void handlePluginSubItemInfoChanged(const QString& subItemID) override;
    void handlePluginSubItemChanged() override;

private:
    KcpPluginInterfaceV2* m_interfaceV2 = nullptr;
};