//
// Created by liuxinhao on 2022/5/31.
//

#include "kcp-plugin-interface.h"
#include "kiran-timedate-global-data.h"
#include "kiran-timedate-widget.h"

#include <qt5-log-i.h>

#include <QTranslator>

class TimedatePlugin : public QObject, public KcpPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterface_iid)
    Q_INTERFACES(KcpPluginInterface)
private:
    ~TimedatePlugin() override = default;
    int init() override
    {
        // TODO:后续考虑全局数据的创建和释放与控件的生存周期绑定
        /// 加载全局数据
        if (!KiranTimeDateGlobalData::instance()->init())
        {
            KLOG_ERROR() << "failed to load time zone information";
            return -1;
        }

        m_translator = new QTranslator;
        if (!m_translator->load(QLocale(),
                                "kiran-cpanel-timedate",
                                ".",
                                "/usr/share/kiran-cpanel-timedate/translations/",
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
        if(m_translator)
        {
            QCoreApplication::removeTranslator(m_translator);
            m_translator->deleteLater();
            m_translator = nullptr;
        }
    }

    QWidget* getSubItemWidget(QString id) override
    {
        QWidget* widget = nullptr;
        if (id == "TimeDate")
        {
            widget = new KiranTimeDateWidget;
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
        return QStringList() << "TimeDate";
    }

private:
    QWidget* m_currentWidget = nullptr;
    QTranslator* m_translator = nullptr;
};

#include "main.moc"