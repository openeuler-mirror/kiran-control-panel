/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
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

#include <QList>
#include <QObject>
#include <QSharedPointer>

#include <kcp-interface.h>
#include <kcp-plugin-subitem.h>
#include <QVector>

// TODO:后续控制中心构建搜索信息时，遍历分类以及子功能项生成搜索信息时，若该分类下只有一个插件的话则隐藏掉子功能项信息的展示
//例如:
// 账户管理分类下只有一个插件提供的一个功能项，则生成的搜索项，为"账户管理"
// 个性化分类下存在多个插件以及多个功能项，则生成的搜索项为，分类+功能项目, "个性化->快捷键"
//
//通过registerCustomSearchEntry(id,searchkey,text)注册，将会在上述的信息之上加入参数中的text
//例如: text=创建账户  账户管理->创建账户
//例如: text=新增快捷键 个性化->快捷键->新增快捷键

class KcpPluginInterfaceV2
{
public:
    virtual ~KcpPluginInterfaceV2(){};

public:
    virtual int init(KcpInterface* interface) = 0;
    virtual void uninit() = 0;

    // 功能项数组，生存周期由插件维护
    // 功能项发生变更时，应调用init时传入KcpInterface接口，通知主面板相关信息变更,及时加载新的功能项信息
    virtual QVector<KcpPluginSubItemPtr> getSubItems() = 0;
};

#define KcpPluginInterfaceV2_iid "com.kylinsec.Kiran.ControlPanelInterface/2.4"
Q_DECLARE_INTERFACE(KcpPluginInterfaceV2, KcpPluginInterfaceV2_iid)
