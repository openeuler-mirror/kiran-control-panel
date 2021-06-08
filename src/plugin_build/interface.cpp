#include "interface.h"
#include "pages/page-mouse/mouse-settings.h"
#include "pages/page-touchpad/touchpad-settings.h"
#include "dbus-interface/touchpad-interface.h"
#include "dbus-interface/mouse-interface.h"
#include "config/config.h"
#include <kiran-message-box.h>
#include <QLocale>
#include <QTranslator>
#include <QCoreApplication>
#include <QFile>

PluginMouseInterface::PluginMouseInterface()
{
    m_touchPadInterface = ComKylinsecKiranSessionDaemonTouchPadInterface::instance();
}

bool PluginMouseInterface::haveUnsavedOptions()
{
    return false;
}

QStringList PluginMouseInterface::visibleSubItems()
{
    QStringList subItems;
    subItems << "MouseSettings";
    if(m_touchPadInterface->has_touchpad())
    {
        subItems << "TouchPadSettings";
    }

    return subItems;
}

QWidget *PluginMouseInterface::getSubItemWidget(QString id)
{
    QWidget* widget = nullptr;
    if (id == "MouseSettings")
    {
        widget = new MouseSettings();
    }
    else if (id == "TouchPadSettings")
    {
        widget = new TouchPadSettings();
    }
    m_currentWidget = widget;
    return m_currentWidget;
}

void PluginMouseInterface::uninit()
{
    if( m_translator )
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
}

int PluginMouseInterface::init()
{
    if(!ComKylinsecKiranSessionDaemonMouseInterface::instance()->isValid() ||
       !ComKylinsecKiranSessionDaemonTouchPadInterface::instance()->isValid())
    {
        return -1;
    }
    //加载翻译文件
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    m_translator = new QTranslator;
    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-mouse",
                            ".",
                             TRANSLATIONS_FILE_DIR,
                            ".qm"))
    {
        qDebug() << "Kiran cpanel mouse load translation failed" << endl;
        delete m_translator;
        m_translator = nullptr;
        return -1;
    }
    else
    {
        QCoreApplication::installTranslator(m_translator);
    }
    return 0;
}

