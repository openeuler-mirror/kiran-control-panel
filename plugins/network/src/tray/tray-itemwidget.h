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
 * Author:     luoqing <luoqing@kylinsec.com.cn>
 */

#ifndef KIRAN_CPANEL_NETWORK_TRAY_ITEMWIDGET_H
#define KIRAN_CPANEL_NETWORK_TRAY_ITEMWIDGET_H

#include <style-palette.h>
#include <QWidget>
QT_BEGIN_NAMESPACE
namespace Ui
{
class TrayItemWidget;
}
QT_END_NAMESPACE

enum TrayItemWidgetStatus
{
    WIDGET_STATUS_SIMPLE,
    WIDGET_STATUS_PREPARE_CONNECT,
    WIDGET_STATUS_ACTIVATING,
    WIDGET_STATUS_ACTIVATED
};

class TrayItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrayItemWidget(QWidget *parent = nullptr);
    ~TrayItemWidget() override;

    void initUI();
    void initConnection();

    void setWidgetsInDifferentStatus(TrayItemWidgetStatus WidgetStatus);

    void setName(const QString &name);
    QString getName();

    void setWirelessStatusIcon(bool security, int signal);
    void setWiredStatusIcon();
    void setOtherNetworkIcon();

    void activatedStatus();
    void deactivateStatus();
    void prepareConnectStatus();
    void simpleStatus();
    void activatingStatus();

    void setLoadingStatus(bool isLoading);
    //    void setLoadingLabelVisible(bool isVisible);
    void setConnectionStatusLabel();
    void setEnableConnectButton(bool enable);

    void setIgnoreAndDisconnectButtonVisible(bool isVisible);
    void setCancelAndConnectButtonVisible(bool isVisible);
    void setIgnoreButtonVisible(bool isVisible);

    void showInputPasswordWidget();
    void showInputSsidWidget();
    TrayItemWidgetStatus itemWidgetStatus();

public slots:
    void handleInputText();
    QString getPassword();
    void handleThemeChanged(Kiran::PaletteType paletteType);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void sendPassword(const QString &password);
    void sendSsid(const QString &ssid);
    void connectButtonClicked();
    void disconnectButttonClicked();
    void ignoreButtonClicked();
    void cancelButtonClicked();
    void clicked();

private:
    Ui::TrayItemWidget *ui;
    TrayItemWidgetStatus m_currentItemWidgetStatus;
};

#endif  // KIRAN_CPANEL_NETWORK_TRAY_ITEMWIDGET_H
