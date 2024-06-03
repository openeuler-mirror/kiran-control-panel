/**
 * Copyright (c) 2022 ~ 2023 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yinhongchang <yinhongchang@kylinsec.com.cn>
 */

#include "language-plugin.h"
#include "language-subitem.h"
#include "language-page.h"

LanguagePlugin::LanguagePlugin(QObject* parent) : QObject(parent)
{
}

LanguagePlugin::~LanguagePlugin()
{
}

int LanguagePlugin::init(KiranControlPanel::PanelInterface* interface)
{
    initSubItem();
    return 0;
}

void LanguagePlugin::uninit()
{
}

QVector<KiranControlPanel::SubItemPtr> LanguagePlugin::getSubItems()
{
    return m_subitems;
}

void LanguagePlugin::initSubItem()
{
    auto languageSubItemCreator = []() -> QWidget*
    {
        return new LanguagePage();
    };

    struct SubItemStruct
    {
        QString id;
        QString name;
        QString category;
        QString desc;
        QString icon;
        int weight;
        CreateWidgetFunc func;
    };

    QList<SubItemStruct> subitemInfos = {
        {"Language",
        tr("Language"),
        "language",
        "",
        ":/kcp-language/images/language.svg",
        96,
        languageSubItemCreator}
    };

    for (const SubItemStruct& subitemInfo : subitemInfos)
    {
        LanguageSubItem* subitem = new LanguageSubItem(subitemInfo.func);

        subitem->setID(subitemInfo.id);
        subitem->setName(subitemInfo.name);
        subitem->setCategory(subitemInfo.category);
        subitem->setDesc(subitemInfo.desc);
        subitem->setIcon(subitemInfo.icon);
        subitem->setWeight(subitemInfo.weight);
        m_subitems.append(KiranControlPanel::SubItemPtr(subitem));
    }
}