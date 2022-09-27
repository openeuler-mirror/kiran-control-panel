#pragma once

#include "kcp-plugin-interface.h"
#include "kcp-plugin-subitem.h"
#include "plugin.h"

class PluginSubItemWrapper;
class PluginV1 : public Plugin
{
    Q_OBJECT
public:
    PluginV1(QObject* parent = nullptr);
    ~PluginV1();

    /// @brief 加载指定路径下的插件desktop文件，通过desktop文件加载相应的插件共享库
    /// @param path 插件desktop文件
    /// @return 是否加载成功
    bool load(const QString& path) override;

    /// @brief 卸载插件
    void unload() override;

    /// @brief 获取插件中的功能项
    /// @return 功能项指针数组，此数组会发生改变，不能缓存
    virtual QVector<KcpPluginSubItemPtr> getSubItems() override;

    QString getLibraryPath();

private:
    /// @brief 从插件desktop文件之中读取插件共享库路径
    /// @param desktopPath 插件desktop文件路径
    /// @return 共享库路径
    QString getLibraryPathFromDesktop(const QString& desktopPath);

    /// @brief 解析Desktop文件信息
    /// @param path Desktop文件路径
    /// @return 是否解析成功
    bool parseDesktopInfo(const QString& desktopPath);

private:
    KcpPluginInterface* m_interface = nullptr;
    QStringList m_visiableSubItems;
    QVector<KcpPluginSubItemPtr> m_subItemVec;
    QString m_libraryPath;
};