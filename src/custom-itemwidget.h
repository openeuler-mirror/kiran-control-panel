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
class CustomItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CustomItemWidget(ItemWidgetType itemWidgetType, QWidget *parent = nullptr);

public:
    void initUI();
    void initPluginItemWidget();
    void initTrayItemWidget();

    void setName(const QString &name);
    QString getName();
    void activatedLabel();
    void deactivateLabel();
    void setLoadingStatus(bool isLoading);
    void setLabelVisible(bool isVisible);
    void setActionButtonVisible(bool isVisible);
    void setWirelessStatusIcon(bool security, int signal);
    void setWiredStatusIcon();

    QPixmap getPixmapFromSvg(const QString &svgPath);


public slots:
    void handleTrayItemActivated();
    void handleTrayItemClicked();

signals:
    void actionButtonClicked();

private:
    QLabel *m_connectionTypeIcon;
    QLabel *m_connectionName;
    QHBoxLayout *m_horizonLayout;
    QHBoxLayout *m_horizonIconAndNameLayout;
    QHBoxLayout *m_horizonActivatedLabelLayout;
    QHBoxLayout *m_horizonConnectionInfoLayout;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *m_verticalLayout;
    QPushButton *m_actionButton;
    AnimationLoadingLabel *m_activatedLabel;
    QWidget *m_activeStatusWidget;
    ItemWidgetType m_itemWidgetType;

};

#endif  // KIRAN_CPANEL_NETWORK_CUSTOMITEMWIDGET_H
