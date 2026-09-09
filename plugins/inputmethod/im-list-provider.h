/**
 * Copyright (c) 2020 ~ 2026 KylinSec Co., Ltd.
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

#ifndef IM_LIST_PROVIDER_H
#define IM_LIST_PROVIDER_H

#include <QVector>

#include "im-entry.h"

/// 「添加输入法」对话框的最小列表数据源抽象（无任何后端概念）。
///
/// 对话框只需"已启用 + 可添加"两组输入法列表即可工作（搜索、多选、返回新增项），
/// 不感知具体后端的 D-Bus / config 细节。任何能提供这两组列表的 controller
/// 实现本接口即可复用同一个 AddImDialog，从而各后端页面共享该对话框。
class ImListProvider
{
public:
    virtual ~ImListProvider() = default;

    /// 后端是否可用（服务在线）。对话框据此做空态提示。
    virtual bool isAvailable() const = 0;

    /// 已启用的输入法列表（含顺序）
    virtual QVector<ImEntry> enabledEntries() = 0;

    /// 可添加的输入法候选（未启用项）
    virtual QVector<ImEntry> availableEntries() = 0;
};

#endif  // IM_LIST_PROVIDER_H
