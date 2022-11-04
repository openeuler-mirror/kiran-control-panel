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

#pragma once

#include <kiran-message-box.h>
#include <kiran-session-daemon/display-i.h>
#include <qt5-log-i.h>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QPushButton>
#include <QVariant>
#include "config-data.h"
#include "display_backEnd_proxy.h"
#include "generic-types.h"
#include "monitor_backEnd_proxy.h"

#define KIRAN_DBUS_INTREFACE_PROPERTIES "org.freedesktop.DBus.Properties"
#define KIRAN_DBUS_PROPERTIES_FUN "PropertiesChanged"
#define KIRAN_SCREEN_COPY_MODE_MONITOR_PATH "kiran.screen.copy.mode.monitor.path"  //复制模式标志，在dbus后台中不存在，仅仅是一个标识而以。

typedef QSharedPointer<DisplayBackEndProxy> DisplayInterface;
typedef QSharedPointer<MonitorBackEndProxy> MonitorInterface;
typedef QList<MonitorInterface> MonitorInterfaceList;

typedef QSharedPointer<DisplayConfigData> DisplayConfigDataPtr;
typedef QSharedPointer<MonitorConfigData> MonitorConfigDataPtr;
typedef QList<MonitorConfigDataPtr> MonitorConfigDataList;

class DisplayConfig : public QObject
{
    Q_OBJECT
private:
    DisplayConfig(QObject *parent = nullptr);

public:
    static DisplayConfig *instance();
    ~DisplayConfig() override;

    MonitorInterfaceList monitorInterfaceList();

    bool isCopyMode();
    MonitorConfigDataPtr initCopyMode();
    MonitorConfigDataList initExtraMode();

    QSharedPointer<DisplayConfigData> getDisplayConfigData();
    QSharedPointer<MonitorConfigData> getMonitorConfigData(const QString &monitorPath);

    void setConfigMode(ConfigMode configMode);
    ConfigMode currentConfigMode();

    bool applyChanges();
    void clear();

    QString primary();
    int windowScalingFactor();
    QStringList listMonitors();
    QList<DisplayModesStu> listModes(const QString &monitorPath);
    QList<DisplayModesStu> listPreferredModes(const QString &monitorPath);

private:
    void init();
    void initConnect();
    DisplayInterface displayInterface();
    MonitorInterface getMonitorInterface(const QString &monitorPath);

private slots:
    void handleSaveBtnClicked();
    void handleCancelBtnClicked();
    void handleDbusPropertiesChanged();

signals:
    void dbusPropertyChanged();
    void saving();
    void saved();
    void canceled();
    void configModeChanged(ConfigMode mode);

private:
    DisplayInterface m_displayInterface;
    QList<MonitorInterface> m_monitorInterfaceList;
    QMap<QString, MonitorInterface> m_monitorInterfaceMap;

    DisplayConfigDataPtr m_displayConfigData;

    MonitorConfigDataList m_monitorDataList;

    QMap<QString, MonitorConfigDataPtr> m_monitorConfigDataMap;

    ConfigMode m_currentConfigMode;
    QTimer m_timer;
};

namespace DBusInterface
{
QVariant Display(const QString &function, const QVariantList &paras = QVariantList(), int *flag = nullptr, const bool &showErrorBox = true);
QVariant DisplayProperty(const char *name);

template <typename T>
T Monitor(const QString &dbusPath, const QString &function, const QVariantList &paras = QVariantList(), const bool &showErrorBox = true, T * = NULL)
{
    QDBusMessage message = QDBusMessage::createMethodCall(DISPLAY_DBUS_NAME, dbusPath, "com.kylinsec.Kiran.SessionDaemon.Display.Monitor",
                                                          function);
    if (!paras.isEmpty()) message.setArguments(paras);
    QDBusMessage response = QDBusConnection::sessionBus().call(message, QDBus::Block, 3000);
    if (response.type() == QDBusMessage::ReplyMessage)
    {
        if (!response.arguments().isEmpty())
            return qdbus_cast<T>(response.arguments().first().value<QDBusArgument>());
    }
    else
    {
        KLOG_ERROR() << "DBus调用失败: "
                     << "\t"
                     << "function: " << function << "\t"
                     << "paras:    " << paras << "\t"
                     << "response: " << response.errorMessage();
        if (showErrorBox)
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
}  // namespace DBusInterface
