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
#ifndef KIRAN_CONTROL_PANEL_OPTIONS_COMBOBOX_H
#define KIRAN_CONTROL_PANEL_OPTIONS_COMBOBOX_H

#include <QComboBox>
class OptionsComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit OptionsComboBox(QWidget *parent = nullptr);
    ~OptionsComboBox() override;
    void wheelEvent(QWheelEvent *e) override;
private:

};

#endif  // KIRAN_CONTROL_PANEL_OPTIONS_COMBOBOX_H
