/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yangxiaoqing <yangxiaoqing@kylinsec.com.cn>
 */

#include "kiran-display-config-global.h"

#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusInterface>
#include <qt5-log-i.h>

QVariant KiranDisplayConfigGlobal::Display(const QString &function, const QVariantList &paras, int *flag, const bool &showErrorBox)
{
    //构造一个method_call消息，服务名称为：com.kscmms.security.center.qtdbus，对象路径为：/message, 接口名称为com.kscmms.security.center.qtdbus.sf，method名称为 setLoginFailedOpr
    QDBusMessage message = QDBusMessage::createMethodCall(KIRAN_DBUS_SERVICE_NAME, KIRAN_DBUS_DISPLAY, KIRAN_DBUS_SERVICE_NAME,
                                                          function);
    if(!paras.isEmpty()) message.setArguments(paras);
    //发送消息
    QDBusMessage response = QDBusConnection::sessionBus().call(message, QDBus::Block, 3000);
    //判断method是否被正确返回
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        //从返回参数获取返回值
        if(!response.arguments().isEmpty())
            return response.arguments().takeFirst();
    }
    else
    {
        KLOG_ERROR() << "dbus interface failed:" << "\t"
                     << "function: " << function << "\t"
                     << "paras:    " << paras    << "\t"
                     << "response: " << response.errorMessage();
        if(flag) *flag = -1;

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

        return response.errorMessage();
    }

    return QVariant();
}

QVariant KiranDisplayConfigGlobal::MonitorProperty(const QString &dbusPath, const char *name)
{
    QDBusInterface remoteApp(KIRAN_DBUS_SERVICE_NAME, dbusPath, "com.kylinsec.Kiran.SessionDaemon.Display.Monitor");
    return remoteApp.property(name);
}

QVariant KiranDisplayConfigGlobal::MonitorSetProperty(const QString &dbusPath, const char *name, const QVariant &value)
{
    QDBusInterface remoteApp(KIRAN_DBUS_SERVICE_NAME, dbusPath, "com.kylinsec.Kiran.SessionDaemon.Display.Monitor");
    return remoteApp.setProperty(name, value);
}

void DisplayModesStu::registerMetaType()
{
    qDBusRegisterMetaType<DisplayModesStu>();
    qDBusRegisterMetaType<ListDisplayModesStu>();
}

QVariant KiranDisplayConfigGlobal::DisplayProperty(const char *name)
{
    QDBusInterface remoteApp(KIRAN_DBUS_SERVICE_NAME, KIRAN_DBUS_DISPLAY, KIRAN_DBUS_SERVICE_NAME);
    return remoteApp.property(name);
}
