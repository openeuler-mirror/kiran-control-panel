/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-appearance is licensed under Mulan PSL v2.
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
#if 1
#include "kcp-plugin-interface.h"
#include "appearance-config.h"
#include "pages/font/fonts.h"
#include "pages/theme/themes.h"
#include "pages/wallpaper/wallpaper.h"

#include <QApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusConnectionInterface>
#include <QTranslator>

#include <kiran-log/qt5-log-i.h>
#include <kiran-session-daemon/appearance-i.h>

#define TRANSLATION_DIR TRANSLATIONS_FILE_DIR

class KcpInterface : public QObject, public KcpPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KcpPluginInterface_iid)
    Q_INTERFACES(KcpPluginInterface)

public:
    KcpInterface() = default;
    ~KcpInterface() = default;

public:
    /**
     * 插件需提供的初始化方法，在其中加载翻译文件或做其他初始化操作
     * \return 初始化返回值 返回0标志成功，其他值标志失败！
     */
    virtual int init() override
    {
        if (!QDBusConnection::sessionBus().interface()->isServiceRegistered(APPEARANCE_DBUS_NAME).value())
        {
            KLOG_INFO() << "Connect appearance dbus service failed!";
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
                                "kiran-cpanel-appearance",
                                ".",
                                TRANSLATION_DIR,
                                ".qm"))
        {
            KLOG_DEBUG() << "Kiran cpanel appearance load translation failed";
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
    virtual void uninit() override
    {
        if (m_translator)
        {
            QCoreApplication::removeTranslator(m_translator);
            delete m_translator;
            m_translator = nullptr;
        }
    }

    /**
     * \brief 通过插件功能项名称(PluginSubItem->name)获取显示控件
     * \param id 功能项ID
     * \return 该功能项的显示控件
     */
    virtual QWidget* getSubItemWidget(QString subItemName) override
    {
        QWidget* widget = nullptr;
        if (subItemName == "Wallpaper")
        {
            widget = new Wallpaper();
        }
        else if (subItemName == "Fonts")
        {
            widget = new Fonts();
        }
        else if (subItemName == "Themes")
        {
            widget = new Themes();
        }
        m_currentWidget = widget;
        return m_currentWidget;
    }

    /**
     * 插件实现该方法用于判断是否存在未保存的设置项,用于提供切换页面时做检查
     * \return 是否存在未保存项
     */
    virtual bool haveUnsavedOptions() override
    {
        return false;
    }

    QStringList visibleSubItems() override
    {
        return {"Themes","Fonts","Wallpaper"};
    }

private:
    QWidget* m_currentWidget = nullptr;
    QTranslator* m_translator = nullptr;
};

#include "main.moc"
#else
#include <kiran-log/qt5-log-i.h>
#include <kiran-message-box.h>
#include <kiran-single-application.h>
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QFile>
#include <QLoggingCategory>
#include <QTranslator>
#include <iostream>
#include "appearance-config.h"
#include "kiran-cpanel-appearance.h"

//TODO:use config.h
#define TRANSLATION_DIR TRANSLATIONS_FILE_DIR

int main(int argc, char *argv[])
{
    //设置日志输出
    QLoggingCategory::defaultCategory()->setEnabled(QtMsgType::QtDebugMsg, true);
    if (klog_qt5_init("", "kylinsec-session", "kiran-cpanel-appearance", "kiran-cpanel-appearance") < 0)
    {
        std::cout << "init klog error" << std::endl;
    }

    KLOG_INFO() << "******New Output*********";

    KiranSingleApplication a(argc, argv);

    //加载翻译文件
    QTranslator *qtTranslator = new QTranslator(qApp);
    if (qtTranslator->load(QLocale(), "kiran-cpanel-appearance", ".", TRANSLATION_DIR, ".qm"))
    {
        a.installTranslator(qtTranslator);
    }
    else
    {
        KLOG_DEBUG("Load Translator File failed : %s\n", TRANSLATION_DIR);
    }

    QFile file(":/themes/style.qss");
    if (file.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(file.readAll());

        a.setStyleSheet(styleSheet);
        file.close();
    }
    else
    {
        KiranMessageBox::message(nullptr, "warning", "Open failed", KiranMessageBox::Ok);
    }

    KiranCpanelAppearance w;
    int screenNum = QApplication::desktop()->screenNumber(QCursor::pos());
    QRect screenGeometry = QApplication::desktop()->screenGeometry(screenNum);
    w.move(screenGeometry.x() + (screenGeometry.width() - w.width()) / 2,
           screenGeometry.y() + (screenGeometry.height() - w.height()) / 2);

    w.resize(980, 760);
    w.show();

    return a.exec();
}
#endif