#include "display-plugin.h"
#include <qt5-log-i.h>
#include <QTranslator>
#include <QApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <kiran-session-daemon/display-i.h>
#include "config.h"
#include "display-subitem.h"

DisplayPlugin::DisplayPlugin(QObject *parent)
    : QObject{parent}
{

}

DisplayPlugin::~DisplayPlugin()
{

}

int DisplayPlugin::init(KiranControlPanel::PanelInterface *interface)
{
    if (!QDBusConnection::sessionBus().interface()->isServiceRegistered(DISPLAY_DBUS_NAME).value())
    {
        KLOG_INFO() << "Connect display dbus service failed!";
        return -1;
    }
    m_translator = new QTranslator;

    if (!m_translator->load(QLocale(),
                            "kiran-cpanel-display",
                            ".",
                            TRANSLATION_DIR_PATH,
                            ".qm"))
    {
        m_translator->deleteLater();
        m_translator = nullptr;
        KLOG_ERROR() << "load translator failed!";
    }
    else
    {
        qApp->installTranslator(m_translator);
    }

    auto displaySubitem = new DisplaySubitem();
    m_subitem.reset(displaySubitem);
    return 0;
}

void DisplayPlugin::uninit()
{
    if (m_translator)
    {
        qApp->removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
}

QVector<KiranControlPanel::SubItemPtr> DisplayPlugin::getSubItems()
{
    return {m_subitem};
}
