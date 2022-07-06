//
// Created by liuxinhao on 2022/6/14.
//
#include "config.h"
#include "upower-interface.h"
#include "pages/general-settings/general-settings-page.h"
#include "pages/power-settings/power-settings-page.h"
#include "pages/battery-settings/battery-settings-page.h"

#include <kcp-plugin-interface.h>
#include <qt5-log-i.h>

#include <QApplication>
#include <QTranslator>

bool addBattery=false;

class KcpPower : public QObject,public KcpPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterface_iid)
    Q_INTERFACES(KcpPluginInterface)

public:
    ~KcpPower() = default;

    int init() override
    {
        if (m_translator != nullptr)
        {
            QCoreApplication::removeTranslator(m_translator);
            delete m_translator;
            m_translator = nullptr;
        }

        m_translator = new QTranslator(qApp);
        if (!m_translator->load(QLocale(),
                                "kiran-cpanel-power",
                                ".",
                                KIRAN_POWER_MANAGER_TRANSLATIONS_DIR,
                                ".qm"))
        {
            KLOG_ERROR() << "can't load translator";
            delete m_translator;
            m_translator = nullptr;
        }
        else
        {
            qApp->installTranslator(m_translator);
        }
        return 0;
    }

    void uninit() override
    {

    }

    QWidget* getSubItemWidget(QString id) override
    {
        QWidget* widget = nullptr;
        if(id == "GeneralSettings")
        {
            widget = new GeneralSettingsPage;
        }
        else if (id == "PowerSettings")
        {
            widget = new PowerSettingsPage;
        }
        else if( id == "BatterySettings")
        {
            widget = new BatterySettingsPage;
        }
        return widget;
    }

    bool haveUnsavedOptions() override
    {
        return false;
    }

    QStringList visibleSubItems() override
    {
        QStringList subItem({{"GeneralSettings"},{"PowerSettings"}});

        if( UPowerInterface::haveBattery() )
        {
            subItem << "BatterySettings";
        }

        return subItem;
    }

private:
    QWidget* m_currentWidget;
    QTranslator* m_translator = nullptr;
};

#include "main.moc"