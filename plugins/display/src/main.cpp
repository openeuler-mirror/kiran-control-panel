#include <QApplication>
#include <QTranslator>

#include <kiran-log/qt5-log-i.h>

#include "kcp-plugin-interface.h"
#include "config.h"
#include "kiran-display-configuration.h"

class QTranslator;
class KcpDisplay : public QObject, public KcpPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterface_iid)
    Q_INTERFACES(KcpPluginInterface)
public:
    KcpDisplay() = default;
    ~KcpDisplay() = default;

    int init() override
    {
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

        return 0;
    }

    void uninit() override
    {
        if (m_translator)
        {
            qApp->removeTranslator(m_translator);
            delete m_translator;
            m_translator = nullptr;
        }
    }

    QWidget *getSubItemWidget(QString id) override
    {
        QWidget* widget = nullptr;

        if (id == "Display")
        {
            widget = new KiranDisplayConfiguration();
        }

        return widget;
    }

    bool haveUnsavedOptions() override
    {
        return false;
    }

    QStringList visibleSubItems() override
    {
        return QStringList() << "Display";
    }

private:
    QTranslator* m_translator = nullptr;
};

#include "main.moc"