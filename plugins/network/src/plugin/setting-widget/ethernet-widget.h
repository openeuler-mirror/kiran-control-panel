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

#ifndef KIRAN_CPANEL_NETWORK_ETHERNET_WIDGET_H
#define KIRAN_CPANEL_NETWORK_ETHERNET_WIDGET_H

#include <QWidget>
#include <NetworkManagerQt/WiredSetting>
QT_BEGIN_NAMESPACE
namespace Ui
{
class EthernetWidget;
}
QT_END_NAMESPACE
class KiranSwitchButton;
using namespace NetworkManager;
class EthernetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EthernetWidget(QWidget *parent = nullptr);
    ~EthernetWidget() override;
    void initUI();
    void initEthernetMacComboBox();
    void initConnection();
    void setWiredSetting(const WiredSetting::Ptr &wiredSetting);
    bool isCloneMacValid(const QString &cloneMac);

public slots:
    void handleCustomMTUChanged(bool checked);
    void saveSettings();
    void showSettings();
    void resetSettings();
    void clearPtr();
    bool isInputValid();

private:
    Ui::EthernetWidget *ui;
    WiredSetting::Ptr m_wiredSetting;
    KiranSwitchButton *m_mtuButton;
};

#endif  //KIRAN_CPANEL_NETWORK_ETHERNET_WIDGET_H
