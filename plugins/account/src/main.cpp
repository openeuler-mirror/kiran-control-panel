#include "kcp-plugin-interface.h"
#include "kiran-account-manager.h"
#include "accounts-global-info.h"
#include "temporary-dir-manager.h"
#include "kcp-account-config.h"

#include <QStyle>
#include <qt5-log-i.h>

#define DEFAULT_THEME ":/kcp-account/themes/kiran-cpanel-account.qss"

class KeybindingPlugin : public QObject,public KcpPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterface_iid)
    Q_INTERFACES(KcpPluginInterface)

public:
    ~KeybindingPlugin()= default;
    int init() override
    {
        if (!AccountsGlobalInfo::instance()->init())
        {
            KLOG_ERROR() << "load user info failed!";
            return -1;
        }
        if (!TemporaryDirManager::instance()->init(qAppName()))
        {
            KLOG_ERROR() << "init temporary dir manager failed!";
            return -1;
        }

        m_translator = new QTranslator;
        if (!m_translator->load(QLocale(),
                                "kiran-cpanel-account",
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

    }

    QWidget* getSubItemWidget(QString subItemName) override
    {
        QWidget* widget = nullptr;
        if(subItemName == "Account")
        {
            widget = new KiranAccountManager;
        }
        m_currentWidget = widget;
        return m_currentWidget;
    }

    bool haveUnsavedOptions() override
    {
        return false;
    }

    QStringList visibleSubItems() override
    {
        return QStringList() << "Account";
    }

private:
    QWidget* m_currentWidget = nullptr;
    QTranslator* m_translator = nullptr;
};
#include "main.moc"

