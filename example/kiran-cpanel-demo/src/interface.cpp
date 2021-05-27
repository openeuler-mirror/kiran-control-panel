/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#include "interface.h"
#include "subitem/subitem-1.h"
#include "subitem/subitem-2.h"
#include "plugin-demo-config.h"

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
                             PLUGIN_TRANSLATION_DIR,
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
