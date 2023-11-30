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
#ifndef __GENERAL_SUBITEM_H__
#define __GENERAL_SUBITEM_H__

#include <QObject>

#include "general-page.h"
#include "panel-interface.h"
#include "plugin-subitem-interface.h"

class GeneralSubItem : public QObject,
                       public KiranControlPanel::PluginSubitemInterface
{
    Q_OBJECT
public:
    GeneralSubItem(QObject* parent = nullptr)
        : QObject(parent)
    {
    }
    ~GeneralSubItem()
    {
    }

public:
    // 功能项ID,用于区分功能项,应确保其唯一
    QString getID() override
    {
        return "KeyboardGeneral";
    }

    // 功能项名称，用于显示在启动器标题栏之中
    QString getName() override
    {
        return tr("Keyboard General Option");
    }

    // 获取功能项分类ID，该功能项属于哪个分类
    QString getCategory() override
    {
        return "hardware";
    }

    // 获取功能项目状态描述，显示在功能项侧边栏右边状态文本描述
    QString getDesc() override
    {
        return "";
    }

    // 获取功能项图标显示，用于形成功能项侧边栏的左侧图标
    QString getIcon() override
    {
        return "kcp-keyboard-generation";
    }

    // 获取功能项权重，用于多个功能项排序
    int getWeight() override
    {
        return 1;
    }

    // 创建显示控件
    QWidget* createWidget() override
    {
        return new GeneralPage();
    }

    // 获取自定义搜索关键词
    //  QVector< 显示文本(已翻译)，搜索跳转标识ID >
    QVector<QPair<QString, QString>> getSearchKeys() override { return {}; };
    // 跳转至自定义搜索项
    bool jumpToSearchEntry(const QString& key) override { return false; };
    // 该功能项是否存在未保存配置
    bool haveUnSavedOptions() override { return false; };
};
#endif