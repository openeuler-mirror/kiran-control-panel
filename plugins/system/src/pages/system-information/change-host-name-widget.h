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

#ifndef CHANGEHOSTNAMEWIDGET_H
#define CHANGEHOSTNAMEWIDGET_H

#include <QWidget>
#include "config/config.h"

namespace Ui
{
class ChangeHostNameWidget;
}

#ifdef DISABLE_KIRANWIDGETS
class ChangeHostNameWidget : public QWidget
#else
#include <kiranwidgets-qt5/kiran-titlebar-window.h>
class ChangeHostNameWidget : public KiranTitlebarWindow
#endif
{
    Q_OBJECT

public:
    explicit ChangeHostNameWidget();
    ~ChangeHostNameWidget();
    bool getLineEditStatus();

private:
    void initUI();

signals:
    void sigChangeNameSuccessful(bool isChanged, QString name);

private slots:
    void setNewHostName();
    void setInputEditStatus();

private:
    Ui::ChangeHostNameWidget *ui;
};

#endif  // CHANGEHOSTNAMEWIDGET_H
