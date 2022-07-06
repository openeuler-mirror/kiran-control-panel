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

#ifndef KIRAN_CPANEL_NETWORK_DSL_SETTING_PAGE_H
#define KIRAN_CPANEL_NETWORK_DSL_SETTING_PAGE_H

#include <QWidget>
#include "setting-page.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class DslSettingPage;
}
QT_END_NAMESPACE

class DslSettingPage : public QWidget
{
    Q_OBJECT

public:
    explicit DslSettingPage(QWidget *parent = nullptr);
    ~DslSettingPage() override;

    void initUI();
    void initConnection();
//    void initSpecificSettings() override;

private:
    Ui::DslSettingPage *ui;
};

#endif  //KIRAN_CPANEL_NETWORK_DSL_SETTING_PAGE_H
