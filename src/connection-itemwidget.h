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

#ifndef KIRAN_CPANEL_NETWORK_CUSTOMITEMWIDGET_H
#define KIRAN_CPANEL_NETWORK_CUSTOMITEMWIDGET_H

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QWidget>

enum ItemWidgetType
{
    ITEM_WIDGET_TYPE_TRAY,
    ITEM_WIDGET_TYPE_PLUGIN
};

class AnimationLoadingLabel;
class ConnectionItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionItemWidget(ItemWidgetType itemWidgetType, QWidget *parent = nullptr);

public:
    void initUI();
    void initPluginItemWidget();
    void initTrayItemWidget();

    void setName(const QString &name);
    QString getName();
    void activatedStatus();
    void deactivateStatus();
    void setLoadingStatus(bool isLoading);
    void setLabelVisible(bool isVisible);
    void setActionButtonVisible(bool isVisible);
    void setWirelessStatusIcon(bool security, int signal);
    void setWiredStatusIcon();
    void setOtherNetworkIcon();

    QPixmap getPixmapFromSvg(const QString &svgPath);


public slots:

signals:
    void actionButtonClicked();

private:
    QLabel *m_connectionTypeIcon;
    QLabel *m_connectionName;
    QLabel *m_connectionStatus;
    QHBoxLayout *m_horizonLayout;
    QHBoxLayout *m_horizonIconAndNameLayout;
    QHBoxLayout *m_horizonActivateStatusLabelLayout;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *m_verticalLayout;
    QPushButton *m_actionButton;
    AnimationLoadingLabel *m_activatedLabel;

    QWidget *m_activeStatusWidget;
    ItemWidgetType m_itemWidgetType;

};

class InputPasswordWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InputPasswordWidget(QWidget *parent= nullptr);

    void intUI();
    void initConnection();

public slots:
    void handleInputPassword();
    QString getPassword();

signals:
    void activateConnectionButtonClicked();
    void sendPassword(const QString& password);
private:
    QPushButton *m_activateConnectionButton;
    QLineEdit *m_passwordEdit;
    QHBoxLayout *m_horizonLayout;

};

#endif  // KIRAN_CPANEL_NETWORK_CUSTOMITEMWIDGET_H
