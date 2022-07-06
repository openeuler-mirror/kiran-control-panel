/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-appearance is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#include "theme-widget-group.h"
#include "theme-widget.h"

ThemeWidgetGroup::ThemeWidgetGroup(QWidget *parent) : QWidget(parent)
{
}

ThemeWidgetGroup::~ThemeWidgetGroup()
{
}

/**
 * @brief ThemeWidgetGroup::addWidget 添加控件
 * @param widget 控件
 */
void ThemeWidgetGroup::addWidget(ThemeWidget *widget)
{
    m_themeWidgets.append(widget);
    connect(widget, &ThemeWidget::clicked, [=] {
        m_preThemeWidget = m_currThemeWidget;
        m_currThemeWidget = widget;

        emit themeWidgetChange(m_preThemeWidget, m_currThemeWidget);
    });
}

void ThemeWidgetGroup::setCurrentWidget(ThemeWidget *widget)
{
    m_currThemeWidget = widget;
}

ThemeWidget *ThemeWidgetGroup::getCurrentWidget()
{
    return m_currThemeWidget;
}

QList<ThemeWidget *> ThemeWidgetGroup::getThemeWidgetList()
{
    return m_themeWidgets;
}
