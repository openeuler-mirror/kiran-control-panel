/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-system is licensed under Mulan PSL v2.
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

#ifndef KIRANSYSTEMINFORMATION_H
#define KIRANSYSTEMINFORMATION_H

#include <QListWidgetItem>
#include <QStackedWidget>
#include <QWidget>
#include "config/config.h"
#include "custom-widgets/information-list-item.h"
namespace Ui
{
class kiranSystemInformation;
}

#ifdef DISABLE_KIRANWIDGETS
class kiranSystemInformation : public QWidget
#else
#include <kiran-titlebar-window.h>
class kiranSystemInformation : public KiranTitlebarWindow
#endif
{
    Q_OBJECT

public:
    explicit kiranSystemInformation(QWidget* parent = 0);
    ~kiranSystemInformation();
    virtual QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent* painEvent);

private:
    void initUI();

private:
    Ui::kiranSystemInformation* ui;
};

#endif  // KIRANSYSTEMINFORMATION_H
