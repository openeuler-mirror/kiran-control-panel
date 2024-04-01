/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinsec.com.cn>
 */

#ifndef KIRAN_CPANEL_NETWORK_CUSTOMITEMWIDGET_H
#define KIRAN_CPANEL_NETWORK_CUSTOMITEMWIDGET_H

#include <kiran-label.h>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Connection>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QWidget>
#include "general.h"
#include "kiran-frame/kiran-frame.h"

class AnimationLoadingLabel;
class ConnectionItemWidget : public KiranFrame
{
    Q_OBJECT
public:
    explicit ConnectionItemWidget(QWidget *parent = nullptr);
    explicit ConnectionItemWidget(NetworkConnectionInfo connectionInfo, QWidget *parent = nullptr);

public:
    void initUI();
    void initPluginItemWidget();

    void setName(const QString &name);
    QString name() { return m_connectionName->text(); };
    void activatedStatus();
    void deactivateStatus();
    void setLoadingStatus(bool isLoading);
    void setLabelVisible(bool isVisible);
    void setEditButtonVisible(bool isVisible);
    void setWirelessStatusIcon(bool security, int signal);
    void setWiredStatusIcon();
    void setOtherNetworkIcon();
    void setMoreOptionsVisible(bool isVisible);
    void setConnectionInfo(NetworkConnectionInfo connectionInfo) { m_connectionInfo = connectionInfo; };
    
    NetworkConnectionInfo connectionInfo() { return m_connectionInfo; };
    int type() { return m_connectionInfo.type; };
    QString uuid() { return m_connectionInfo.uuid; };
    QString connectionPath() { return m_connectionInfo.connectionPath; };
    QString ssid() { return m_connectionInfo.wirelessInfo.ssid; };
    int signalStrength() { return m_connectionInfo.wirelessInfo.signalStrength; };
    QString activeConnectionPath() { return m_connectionInfo.activeConnectionPath; };
    
    void setActiveConnectionPath(QString path) { m_connectionInfo.activeConnectionPath = path; };
    void setActiveStatus(NetworkManager::ActiveConnection::State state);

public slots:
    void activeConnectionStateChanged(NetworkManager::ActiveConnection::State state);
    void disconnectConnection();

private slots:
    void handleThemeChanged(Kiran::PaletteType paletteType);
    void editConnection();
    void updateConnection();
    void removeConnection();
    void activateConnection(const QString &connectionPath = QString(), const QString &connectionParameter = QString());

    void activateHiddenNetwork(const QString &ssid);
    void ignoreWirelessNetwork();

    void setPskAndActivateWirelessConnection(const QString &password);

private:
    void requireInputPassword();
    void requireHiddenNetworkName();

    void addAndActivateWirelessConnection(NetworkManager::ConnectionSettings::Ptr connectionSettings);
    void activateWirelessNetwork();

signals:
    void editButtonClicked();
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QLabel *m_connectionTypeIcon;
    KiranLabel *m_connectionName;
    QLabel *m_connectionStatus;
    QHBoxLayout *m_horizonLayout;
    QHBoxLayout *m_horizonEditAndMoreOptions;
    QHBoxLayout *m_horizonActivateStatusLabelLayout;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *m_verticalLayout;
    QPushButton *m_editButton;
    AnimationLoadingLabel *m_activatedLabel;

    QWidget *m_activeStatusWidget;
    QPushButton *m_moreOptions;
    QMenu *m_menu;
    QAction *m_disconnectAction;
    QAction *m_removeAction;
    QAction *m_ignoreAction;

    NetworkConnectionInfo m_connectionInfo;
    NetworkManager::Connection::Ptr m_connection;
    NetworkManager::ConnectionSettings::Ptr m_connnectionSettings;
};

#endif  // KIRAN_CPANEL_NETWORK_CUSTOMITEMWIDGET_H
