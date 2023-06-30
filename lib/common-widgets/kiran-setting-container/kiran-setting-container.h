/**
 * Copyright (c) 2023 ~ 2024 KylinSec Co., Ltd.
 * ks-sc is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yinhongchang <yinhongchang@kylinsec.com.cn>
 */

#pragma once
#include <QWidget>

QT_BEGIN_NAMESPACE
class QBoxLayout;
QT_END_NAMESPACE

class SettingsContainer : public QWidget
{
    Q_OBJECT
public:
    SettingsContainer(QWidget* parent = nullptr);
    virtual ~SettingsContainer();

    void addKiranSettingItem(QWidget* widget);
    void clear();
    void delAllItemOfLayout(QLayout* layout);

private:
    void init();
    void adjustSizeToItmes();

private:
    QBoxLayout* m_mainLayout;
    QBoxLayout* m_containerLayout;
};
