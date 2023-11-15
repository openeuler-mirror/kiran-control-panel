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
    m_subitems.append(KiranControlPanel::SubItemPtr(new HardwareSubItem));
    m_subitems.append(KiranControlPanel::SubItemPtr(new SystemInfoSubItem));
    return 0;
}

void SystemInfoPlugin::uninit()
{
}

QVector<KiranControlPanel::SubItemPtr> SystemInfoPlugin::getSubItems()
{
    return m_subitems;
}
