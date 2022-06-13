/**
* Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
* kiran-cpanel-keyboard is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*
* Author:     yuanxing <yuanxing@kylinos.com.cn>
*/

#include <kcp-plugin-interface.h>
#include <kiran-session-daemon/keyboard-i.h>
#include <kiran-message-box.h>
#include <qt5-log-i.h>

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QApplication>
#include <QTranslator>
#include <QFile>

#include "general-page.h"
#include "layout-page.h"

class KeyboardPlugin : public QObject, public KcpPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterface_iid)
    Q_INTERFACES(KcpPluginInterface)
public:
    KeyboardPlugin():QObject(){};
    ~KeyboardPlugin(){};

public:
    virtual int init() override
    {
        if (!QDBusConnection::sessionBus().interface()->isServiceRegistered(KEYBOARD_DBUS_NAME).value())
        {
            KLOG_INFO() << "Connect keyboard dbus service failed!";
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
                                "kiran-cpanel-keyboard",
                                ".",
                                "/usr/share/kiran-cpanel-keyboard/translations/",
                                ".qm"))
        {
            KLOG_DEBUG() << "Kiran cpanel keyboard load translation failed";
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

    virtual QWidget* getSubItemWidget(QString subItemName) override
    {
        QWidget* widget = nullptr;
        if (subItemName == "GeneralOptions")
        {
            widget = new GeneralPage;
        }
        else if (subItemName == "KeyboardLayout")
        {
            widget = new LayoutPage();
        }
        m_currentWidget = widget;

        return m_currentWidget;
    }

    virtual bool haveUnsavedOptions() override
    {
        return false;
    }

    QStringList visibleSubItems() override
    {
        return QStringList() << "GeneralOptions" << "KeyboardLayout";
    }
private:
    QWidget* m_currentWidget = nullptr;
    QTranslator* m_translator = nullptr;
};

#include "main.moc"