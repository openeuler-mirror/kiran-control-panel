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

#ifndef KIRAN_CPANEL_NETWORK_DSL_MANAGER_H
#define KIRAN_CPANEL_NETWORK_DSL_MANAGER_H

#include <QWidget>
#include "manager.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
class DslManager;
}
QT_END_NAMESPACE

class DslManager : public Manager
{
    Q_OBJECT

public:
    explicit DslManager(QWidget *parent = nullptr);
    ~DslManager() override;

    void initUI();
    void initConnection();


private:
    Ui::DslManager *ui;
};

#endif  //KIRAN_CPANEL_NETWORK_DSL_MANAGER_H
