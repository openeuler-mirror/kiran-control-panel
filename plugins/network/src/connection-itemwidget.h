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
#include "kiran-frame/kiran-frame.h"

class AnimationLoadingLabel;
class ConnectionItemWidget : public KiranFrame
{
    Q_OBJECT
public:
    explicit ConnectionItemWidget(QWidget *parent = nullptr);

public:
    void initUI();
    void initPluginItemWidget();

    void setName(const QString &name);
    QString getName();
    void activatedStatus();
    void deactivateStatus();
    void setLoadingStatus(bool isLoading);
    void setLabelVisible(bool isVisible);
    void setEditButtonVisible(bool isVisible);
    void setWirelessStatusIcon(bool security, int signal);
    void setWiredStatusIcon();
    void setOtherNetworkIcon();

    QPixmap getPixmapFromSvg(const QString &svgPath);

public slots:

signals:
    void editButtonClicked();

private:
    QLabel *m_connectionTypeIcon;
    QLabel *m_connectionName;
    QLabel *m_connectionStatus;
    QHBoxLayout *m_horizonLayout;
    QHBoxLayout *m_horizonIconAndNameLayout;
    QHBoxLayout *m_horizonActivateStatusLabelLayout;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *m_verticalLayout;
    QPushButton *m_editButton;
    AnimationLoadingLabel *m_activatedLabel;

    QWidget *m_activeStatusWidget;
};

#endif  // KIRAN_CPANEL_NETWORK_CUSTOMITEMWIDGET_H
