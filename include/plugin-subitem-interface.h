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
    //获取功能项ID,用于区分功能项,应确保其唯一
    virtual QString getID() = 0;

    //获取功能项名称，用于显示在启动器标题栏之中
    virtual QString getName() = 0;

    //获取功能项分类ID，该功能项属于哪个分类
    virtual QString getCategory() = 0;

    //获取功能项目状态描述，显示在功能项侧边栏右边状态文本描述
    virtual QString getDesc() = 0;

    //获取功能项图标显示，用于形成功能项侧边栏的左侧图标
    virtual QString getIcon() = 0;

    //获取功能项权重，用于多个功能项排序
    virtual int getWeight() = 0;

    //获取自定义搜索关键词
    // QVector< 显示文本(已翻译)，搜索跳转标识ID >
    virtual QVector<QPair<QString, QString>> getSearchKeys() = 0;

    //创建显示控件
    virtual QWidget* createWidget() = 0;

    //跳转至自定义搜索项
    virtual bool jumpToSearchEntry(const QString& key) = 0;

    //该功能项是否存在未保存配置
    virtual bool haveUnSavedOptions() = 0;
};
typedef QSharedPointer<PluginSubitemInterface> SubItemPtr;
}  // namespace KiranControlPanel
