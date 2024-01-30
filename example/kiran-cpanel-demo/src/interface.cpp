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
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#include "interface.h"
#include "subitem/subitem1.h"
#include "subitem/subitem2.h"
#include "config.h"

#include <QCoreApplication>
#include <QList>
#include <QLocale>
#include <QTranslator>

QWidget* PluginDemoInterface::getSubItemWidget(QString subItemName)
{
    QWidget* widget = nullptr;
    if (subItemName == "SubItem1")
    {
        widget = new SubItem1();
    }
    else if (subItemName == "SubItem2")
    {
        widget = new SubItem2;
    }
    m_currentWidget = widget;
    return m_currentWidget;
}

bool PluginDemoInterface::haveUnsavedOptions()
{
    if (!m_currentWidget)
    {
        return false;
    }

    bool unsaved = m_currentWidget->property("unsaved").toBool();
    return unsaved;
}

int PluginDemoInterface::init()
{
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    m_translator = new QTranslator;
    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-demo",
                            ".",
                             TRANSLATE_PREFIX,
                            ".qm"))
    {
        return -1;
    }
    return 0;
}

void PluginDemoInterface::uninit()
{
    if( m_translator )
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
}

QStringList PluginDemoInterface::visibleSubItems()
{
    return QStringList() << "SubItem1" << "SubItem2";
}