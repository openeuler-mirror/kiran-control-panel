/**
* Copyright (c) 2021 ~ 2022 KylinSec Co., Ltd.
* kiran-cpanel-mouse is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*
* Author:     liuxinhao <liuxinhao@kylinos.com.cn>
*/
#include "kcm-manager.h"
#include "mouse-page.h"
#include "touchpad-page.h"
#include "touchPad_backEnd_proxy.h"

#include <kcp-plugin-interface.h>
#include <kiran-session-daemon/mouse-i.h>
#include <kiran-session-daemon/touchpad-i.h>
#include <qt5-log-i.h>

#include <QTranslator>
#include <QApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>

class KcpMouse : public QObject, public KcpPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterface_iid)
    Q_INTERFACES(KcpPluginInterface)

public:
    KcpMouse();
    ~KcpMouse(){};

public:
    /**
     * 插件需提供的初始化方法，在其中加载翻译文件或做其他初始化操作
     * \return 初始化返回值 返回0标志成功，其他值标志失败！
     */
    int init() override
    {
        m_kcmManager = new KCMManager;
        if (!QDBusConnection::sessionBus().interface()->isServiceRegistered(MOUSE_DBUS_NAME).value() &&
            !QDBusConnection::sessionBus().interface()->isServiceRegistered(TOUCHPAD_DBUS_NAME).value())
        {
            KLOG_INFO() << "Connect mouse or touchpad dbus service failed!";
            return -1;
        }

        QSharedPointer<TouchPadBackEndProxy> touchPadInterface = m_kcmManager->getTouchPadInterface();
        m_hasTouchPad = touchPadInterface->has_touchpad();
        touchPadInterface.clear();

        if (!m_hasTouchPad)
            KLOG_INFO() << "system don't have touchPad";
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
                                "/usr/share/applications/kiran-cpanel-mouse",
                                ".qm"))
        {
            KLOG_DEBUG() << "Kiran cpanel mouse load translation failed";
            m_translator->deleteLater();
            m_translator = nullptr;
        }
        else
        {
            QCoreApplication::installTranslator(m_translator);
        }
        return 0;
    }

    /**
     * 插件需提供取消初始化的操作，在其中对翻译文件进行卸载或取消其他初始化操作
     */
    void uninit() override
    {
        if (m_translator)
        {
            QCoreApplication::removeTranslator(m_translator);
            delete m_translator;
            m_translator = nullptr;
        }
        if (m_kcmManager)
        {
            delete m_kcmManager;
            m_kcmManager = nullptr;
        }
    }

    /**
     * \brief 通过插件功能项名称(PluginSubItem->name)获取显示控件
     * \param id 功能项ID
     * \return 该功能项的显示控件
     */
    QWidget* getSubItemWidget(QString id) override
    {
        QWidget* widget = nullptr;
        if (id == "MouseSettings")
        {
            widget = new MousePage();
        }
        else if (id == "TouchPadSettings")
        {
            widget = new TouchPadPage();
        }
        m_currentWidget = widget;
        return m_currentWidget;
    }

    /**
     * 插件实现该方法用于判断是否存在未保存的设置项,用于提供切换页面时做检查
     * \return 是否存在未保存项
     */
    bool haveUnsavedOptions() override
    {
        return false;
    }

    QStringList visibleSubItems() override
    {
        QStringList subItems;
        subItems << "MouseSettings";
        if (m_hasTouchPad)
        {
            subItems << "TouchPadSettings";
        }
        return subItems;
    }

private:
    QWidget* m_currentWidget = nullptr;
    QTranslator* m_translator = nullptr;
    KCMManager* m_kcmManager = nullptr;
    bool m_hasTouchPad = false;
};

#include "main.moc"