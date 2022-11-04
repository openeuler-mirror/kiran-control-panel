#include "systeminfo-plugin.h"

#include "hardware-subitem.h"
#include "system-subitem.h"

#include <kiran-log/qt5-log-i.h>
#include <QCoreApplication>
#include <QTranslator>

#define TRANSLATION_DIR "/usr/share/kiran-control-panel/translations"

SystemInfoPlugin::SystemInfoPlugin(QObject* parent)
    : QObject(parent)
{
}

SystemInfoPlugin::~SystemInfoPlugin()
{
}

int SystemInfoPlugin::init(KiranControlPanel::PanelInterface* interface)
{
    QString systemInfo;
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }

    m_translator = new QTranslator;
    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-system",
                            ".",
                            TRANSLATION_DIR,
                            ".qm"))
    {
        KLOG_DEBUG() << "Kiran cpanel system load translation failed";
        m_translator->deleteLater();
        m_translator = nullptr;
    }
    else
    {
        QCoreApplication::installTranslator(m_translator);
    }

    m_subitems.append(KiranControlPanel::SubItemPtr(new HardwareSubItem));
    m_subitems.append(KiranControlPanel::SubItemPtr(new SystemInfoSubItem));
    return 0;
}

void SystemInfoPlugin::uninit()
{
    if (m_translator != nullptr)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
}

QVector<KiranControlPanel::SubItemPtr> SystemInfoPlugin::getSubItems()
{
    return m_subitems;
}
