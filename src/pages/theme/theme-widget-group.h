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

#ifndef THEMEWIDGETGROUP_H
#define THEMEWIDGETGROUP_H
#include <QObject>
#include <QWidget>
class ThemeWidget;
class ThemeWidgetGroup : public QWidget
{
    Q_OBJECT
public:
    explicit ThemeWidgetGroup(QWidget* parent = 0);
    ~ThemeWidgetGroup();

    void addWidget(ThemeWidget* widget);
    void setCurrentWidget(ThemeWidget*);

signals:
    void themeWidgetChange(ThemeWidget*, ThemeWidget*);

private:
    ThemeWidget* m_currThemeWidget = nullptr;
    ThemeWidget* m_preThemeWidget = nullptr;
};

#endif  // THEMEWIDGETGROUP_H
