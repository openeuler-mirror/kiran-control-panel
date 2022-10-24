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
#include <QSharedPointer>
#include <QString>

class QWidget;

namespace KiranControlPanel
{
/// @brief 控制中心新版本功能项接口
class PluginSubitemInterface
{
public:
    virtual QString getID() = 0;
    virtual QString getName() = 0;
    virtual QString getCategory() = 0;
    virtual QString getDesc() = 0;
    virtual QString getIcon() = 0;
    virtual int getWeight() = 0;
    /// @brief 获取自定义搜索关键词
    /// @return QVector< 显示文本(已翻译)，搜索跳转标识ID >
    virtual QVector<QPair<QString, QString>> getSearchKeys() = 0;

    virtual QWidget* createWidget() = 0;
    virtual bool jumpToSearchEntry(const QString& key) = 0;
    virtual bool haveUnSavedOptions() = 0;
};
typedef QSharedPointer<PluginSubitemInterface> SubItemPtr;
}  // namespace KiranControlPanel
