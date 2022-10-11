/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
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

#include "kcp-plugin-interface.h"
#include "dbus-wrapper/system-info-dbus.h"
#include "pages/system-information/system-information.h"
#include "pages/hardware-information/hardware-information.h"

#include <kiran-log/qt5-log-i.h>
#include <kiranwidgets-qt5/kiran-message-box.h>

#include <QTranslator>
#include <QCoreApplication>

#define TRANSLATION_DIR "/usr/share/kiran-cpanel-system/translations"

class KcpSystemInformation : public  QObject,public  KcpPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterface_iid)
    Q_INTERFACES(KcpPluginInterface)

public:
    KcpSystemInformation() = default;
    ~KcpSystemInformation(){};

public:
    virtual int init() override
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
        return 0;
    }

    virtual void uninit() override
    {
        if (m_translator)
        {
            QCoreApplication::removeTranslator(m_translator);
            delete m_translator;
            m_translator = nullptr;
        }
    }

    virtual QWidget* getSubItemWidget(QString id) override
    {
        QWidget* widget = nullptr;
        if (id == "SystemInformation")
        {
            widget = new SystemInformation();
        }
        else if (id == "HardwareInformation")
        {
            widget = new HardwareInformation();
        }
        m_currentWidget = widget;
        return m_currentWidget;
    }

    virtual bool haveUnsavedOptions()  override
    {
        return false;
    }

    QStringList visibleSubItems() override
    {
        return QStringList() << "SystemInformation" << "HardwareInformation";
    }

private:
    QWidget* m_currentWidget = nullptr;
    QTranslator* m_translator = nullptr;
};



#include "main.moc"