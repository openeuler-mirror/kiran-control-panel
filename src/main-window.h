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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <QWidget>
enum
{
    KCP_LIST_ITEM_GENERAL,
    KCP_LIST_ITEM_LAYOUT
};

namespace Ui
{
class MainWindow;
}

class MainWindow : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void addSidebarItem(QString text, QString icon);

private:
    Ui::MainWindow *ui;
};

#endif  // MAINWINDOW_H
