/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-keybinding is licensed under Mulan PSL v2.
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

#include "kcp-plugin-interface.h"
#include "shortcut.h"

#include <kiran-log/qt5-log-i.h>
#include <kiran-session-daemon/keybinding-i.h>

#include <QApplication>
#include <QTranslator>
#include <QDBusConnection>
#include <QDBusConnectionInterface>

class kcpKeybinding : public QObject, public KcpPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterface_iid)
    Q_INTERFACES(KcpPluginInterface)

public:
    kcpKeybinding()=default;
    ~kcpKeybinding(){};

public:
    /**
     * 插件需提供的初始化方法，在其中加载翻译文件或做其他初始化操作
     * \return 初始化返回值 返回0标志成功，其他值标志失败！
     */
    int init() override
    {
        if (!QDBusConnection::sessionBus().interface()->isServiceRegistered(KEYBINDING_DBUS_NAME).value())
        {
            KLOG_INFO() << "Connect keybinding dbus service failed!";
            return -1;
        }
        m_translator = new QTranslator;
        if (!m_translator->load(QLocale(),
                            "kiran-cpanel-keybinding",
                            ".",
                            "/usr/share/kiran-cpanel-keybinding/translations/",
                            ".qm"))
        {
            KLOG_DEBUG() << "Kiran cpanel keybinding load translation failed";
            m_translator->deleteLater();
            m_translator = nullptr;
        }
        else
        {
            QCoreApplication::installTranslator(m_translator);
        }
        return 0;
    };

    /**
     * 插件需提供取消初始化的操作，在其中对翻译文件进行卸载或取消其他初始化操作
     */
    void uninit() override
    {
    }

    /**
     * \brief 通过插件功能项名称(PluginSubItem->name)获取显示控件
     * \param id 功能项ID
     * \return 该功能项的显示控件
     */
    QWidget* getSubItemWidget(QString subItemName) override
    {
        QWidget* widget = nullptr;
        if (subItemName == "Keybinding")
        {
            widget = new Shortcut;
        }
        m_currentWidget = widget;

//        //加载样式表
//        QFile file(":/style/style.qss");
//        if (file.open(QFile::ReadOnly))
//        {
//            QString styleSheet = QLatin1String(file.readAll());
//
//            m_currentWidget->setStyleSheet(styleSheet);
//            file.close();
//        }
//        else
//        {
//            KiranMessageBox::message(nullptr, tr("Warning"), tr("Load qss failed"), KiranMessageBox::Ok);
//        }
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
        return QStringList() << "Keybinding";
    }

private:
    QWidget* m_currentWidget = nullptr;
    QTranslator* m_translator = nullptr;
};

#include "main.moc"