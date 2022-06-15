/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#ifndef KIRAN_CONTROL_PANEL_INCLUDE_KCP_PLUGIN_INTERFACE_H_
#define KIRAN_CONTROL_PANEL_INCLUDE_KCP_PLUGIN_INTERFACE_H_

#include <QString>
#include <QWidget>

/// NOTE:
/// 插件信号功能：
/// 部分接口插件通过定义信号来进行传递给控制中心主面板
/// 1. void visibleSubItemsChanged(),通知控制面板刷新侧边栏子项

//插件抽象接口
class KcpPluginInterface
{
public:
    virtual ~KcpPluginInterface(){};

public:
    /**
     * 插件需提供的初始化方法，在其中加载翻译文件或做其他初始化操作
     * \return 初始化返回值 返回0标志成功，其他值标志失败！
     */
    virtual int init() = 0;

    /**
     * 插件需提供取消初始化的操作，在其中对翻译文件进行卸载或取消其他初始化操作
     */
    virtual void uninit() = 0;

    /**
     * \brief 通过插件功能项名称(PluginSubItem->name)获取显示控件
     * \param id 功能项ID
     * \return 该功能项的显示控件
     */
    virtual QWidget* getSubItemWidget(QString id) = 0;

    /**
     * 插件实现该方法用于判断是否存在未保存的设置项,用于提供切换页面时做检查
     * \return 是否存在未保存项
     */
    virtual bool haveUnsavedOptions() = 0;

    /**
     * 获取应该显示的子功能项
     * \param id 功能项ID
     * \return 显示的子功能项列表
     */
    virtual QStringList visibleSubItems() = 0;
};

#define KcpPluginInterface_iid "com.kylinsec.Kiran.ControlPanelInterface/1.0"
Q_DECLARE_INTERFACE(KcpPluginInterface,KcpPluginInterface_iid)

#endif  //KIRAN_CONTROL_PANEL_INCLUDE_KCP_PLUGIN_INTERFACE_H_
