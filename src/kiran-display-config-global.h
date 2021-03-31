#ifndef KIRANDISPLAYCONFIGGLOBAL_H
#define KIRANDISPLAYCONFIGGLOBAL_H

#include "kiranwidgets-qt5/kiran-message-box.h"
#include "zlog_ex.h"
#include <QPushButton>
#include <QVariant>
#include <QString>
#include <QDBusArgument>
#include <QDBusMetaType>
#include <QDBusMessage>
#include <QDBusConnection>
#define KIRAN_DBUS_SERVICE_NAME "com.kylinsec.Kiran.SessionDaemon.Display"
#define KIRAN_DBUS_DISPLAY "/com/kylinsec/Kiran/SessionDaemon/Display"
#define KIRAN_DBUS_INTREFACE_PROPERTIES "org.freedesktop.DBus.Properties"
#define KIRAN_DBUS_PROPERTIES_FUN "PropertiesChanged"
#define KIRAN_SCREEN_COPY_MODE_MONITOR_PATH "kiran.screen.copy.mode.monitor.path" //复制模式标志，在dbus后台中不存在，仅仅是一个标识而以。

struct DisplayModesStu
{
    DisplayModesStu():w(0),h(0),refreshRate(0){}
    uint32_t index;
    uint32_t w;
    uint32_t h;
    double refreshRate;

    friend QDBusArgument &operator<<(QDBusArgument &argument, const DisplayModesStu &stu)
    {
        argument.beginStructure();
        argument << stu.index << stu.w << stu.h << stu.refreshRate;
        argument.endStructure();
        return argument;
    }

    friend const QDBusArgument &operator>>(const QDBusArgument &argument, DisplayModesStu &stu)
    {
        argument.beginStructure();
        argument >> stu.index >> stu.w >> stu.h >> stu.refreshRate;
        argument.endStructure();
        return argument;
    }

    static void registerMetaType();
};
typedef QList<DisplayModesStu> ListDisplayModesStu;
Q_DECLARE_METATYPE(DisplayModesStu)
Q_DECLARE_METATYPE(ListDisplayModesStu)

namespace KiranDisplayConfigGlobal
{
QVariant Display(const QString &function, const QVariantList &paras=QVariantList(), int *flag=nullptr, const bool &showErrorBox=true);
QVariant DisplayProperty( const char *name);

template<typename T>
T Monitor(const QString &dbusPath, const QString &function, const QVariantList &paras=QVariantList(), const bool &showErrorBox=true, T * = NULL)
{
    QDBusMessage message = QDBusMessage::createMethodCall(KIRAN_DBUS_SERVICE_NAME, dbusPath, "com.kylinsec.Kiran.SessionDaemon.Display.Monitor",
                                                          function);
    if(!paras.isEmpty()) message.setArguments(paras);
    QDBusMessage response = QDBusConnection::sessionBus().call(message, QDBus::Block, 3000);
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        if(!response.arguments().isEmpty())
            return qdbus_cast<T>(response.arguments().first().value<QDBusArgument>());
    }
    else
    {
        dzlog_cerr("DBus调用失败: [%s], [%s], [%s].", function, paras, response.errorMessage());
        if(showErrorBox)
        {
            KiranMessageBox box;
            box.setTitle(QObject::tr("Tips"));

            QPushButton btn;
            btn.setText(QObject::tr("OK(K)"));
            btn.setFixedSize(QSize(200, box.buttonSize().height()));
            btn.setShortcut(Qt::CTRL + Qt::Key_K);
            box.addButton(&btn, QDialogButtonBox::AcceptRole);
            box.setText(response.errorMessage());
            box.exec();
        }
    }

    return T();
}

QVariant MonitorProperty(const QString &dbusPath, const char *name);
QVariant MonitorSetProperty(const QString &dbusPath, const char *name, const QVariant &value);
}

#endif // KIRANDISPLAYCONFIGGLOBAL_H
