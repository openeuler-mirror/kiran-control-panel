/**
 * Copyright (c) 2020 ~ 2023 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */
#pragma once
#include <QWidget>

QT_BEGIN_NAMESPACE
class QBoxLayout;
QT_END_NAMESPACE

class AuthSettingContainer : public QWidget
{
    Q_OBJECT
public:
    AuthSettingContainer(QWidget* parent = nullptr);
    virtual ~AuthSettingContainer();

    void addAuthSettingItem(QWidget* widget);
    void clear();

private:
    void init();
    void adjustSizeToItmes();

private:
    QBoxLayout* m_mainLayout;
    QBoxLayout* m_containerLayout;
};