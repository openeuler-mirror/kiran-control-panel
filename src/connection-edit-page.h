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

#ifndef KIRAN_CPANEL_NETWORK_CONNECTION_EDIT_PAGE_H
#define KIRAN_CPANEL_NETWORK_CONNECTION_EDIT_PAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class ConnectionEditPage;
}
QT_END_NAMESPACE
class KiranSwitchButton;
class ConnectionEditPage : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionEditPage(QWidget *parent = nullptr);
    ~ConnectionEditPage() override;
    void initUI();

    void setTitle(QString title);
    void setSwitchButtonVisible(bool visible);


signals:
    void requestCreatConnection();

private:
    Ui::ConnectionEditPage *ui;
    KiranSwitchButton *m_switchButton;
};

#endif  //KIRAN_CPANEL_NETWORK_CONNECTION_EDIT_PAGE_H
