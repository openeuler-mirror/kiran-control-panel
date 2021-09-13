#include "kcp-interface.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-message-box.h>
#include <QCoreApplication>
#include <QFile>
#include <QLocale>
#include <QTranslator>
#include "config/config.h"
#include "dbus-wrapper/dbus-wrapper.h"
#include "general-page.h"
#include "layout-page.h"

KcpInterface::KcpInterface()
{
}

bool KcpInterface::haveUnsavedOptions()
{
    return false;
}

QStringList KcpInterface::visibleSubItems()
{
    QStringList subItems;
    subItems << "GeneralOptions"
             << "KeyboardLayout";

    return subItems;
}

QWidget* KcpInterface::getSubItemWidget(QString id)
{
    QWidget* widget = nullptr;
    if (id == "GeneralOptions")
    {
        widget = new GeneralPage;
    }
    else if (id == "KeyboardLayout")
    {
        widget = new LayoutPage();
    }
    m_currentWidget = widget;

    //加载样式表
    QFile file(":/style/style.qss");
    if (file.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(file.readAll());

        m_currentWidget->setStyleSheet(styleSheet);
        file.close();
    }
    else
    {
        KiranMessageBox::message(nullptr, "warning", "Open failed", KiranMessageBox::Ok);
    }
    return m_currentWidget;
}

void KcpInterface::uninit()
{
    if (m_translator)
    {
        QCoreApplication::removeTranslator(m_translator);
        delete m_translator;
        m_translator = nullptr;
    }
    if (m_dbusWrapper)
    {
        delete m_dbusWrapper;
        m_dbusWrapper = nullptr;
    }
}

int KcpInterface::init()
{
    m_dbusWrapper = new DbusWrapper;
    if (!m_dbusWrapper->isValidConnect())
    {
        KLOG_DEBUG() << "Connect keyboard dbus service failed!";
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
                            TRANSLATIONS_FILE_DIR,
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
