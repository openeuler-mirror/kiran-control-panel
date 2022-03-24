/**
 * Copyright (c) 2022 KylinSec Co., Ltd.
 * kiran-cpanel-network is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     luoqing <luoqing@kylinos.com.cn>
 */

#ifndef KIRAN_CPANEL_NETWORK_CONNECTION_SHOW_PAGE_H
#define KIRAN_CPANEL_NETWORK_CONNECTION_SHOW_PAGE_H

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Manager>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QListWidgetItem>
QT_BEGIN_NAMESPACE
namespace Ui
{
class ConnectionShowPage;
}
QT_END_NAMESPACE
using namespace NetworkManager;
class KiranSwitchButton;

struct ConnectionInfo{
    QString uuid;
    QString connectionPath;
    QString activeConnectionPath;
    NetworkManager::Status status;
};

Q_DECLARE_METATYPE(ConnectionInfo);
Q_DECLARE_METATYPE(NetworkManager::Status)

class ConnectionShowPage : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionShowPage(QWidget *parent = nullptr);
    ~ConnectionShowPage() override;
    void initUI();
    void initConnect();

    void setTitle(QString title);
    void setSwitchButtonVisible(bool visible);

    void addConnectionToLists(Connection::Ptr ptr);
    void updateItemActivatedPath(QListWidgetItem* item,QString activatedPath="");

public slots:
    void clearConnectionLists();
    void handleConnectionItemClicked(QListWidgetItem* item);
    void updateActivatedConnectionInfo(QString activatedPath);

signals:
    void requestCreatConnection();
    void requestEditConnection(const QString& uuid, QString activeConnectionPath);
    void requestActivateCurrentItemConnection(const QString &connectionPath);

private:
    Ui::ConnectionShowPage *ui;
    KiranSwitchButton *m_switchButton;
    QListWidgetItem* m_previousActivatedItem;

};

class ItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ItemWidget( QWidget *parent = nullptr);

public:
    void setName(const QString &name);
    void activatedLabel();
    void deactivateLabel();

signals:
    void editConnectionClicked();

private:
    QLabel *m_connectionName;
    QHBoxLayout *m_horizonLayout;
    QPushButton *m_editConnection;
    QLabel *m_activatedLabel;
};

#endif  //KIRAN_CPANEL_NETWORK_CONNECTION_SHOW_PAGE_H
