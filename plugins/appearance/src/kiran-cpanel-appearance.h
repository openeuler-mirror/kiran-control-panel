/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */

#ifndef KIRANCPANELAPPEARANCE_H
#define KIRANCPANELAPPEARANCE_H
#include <kiran-titlebar-window.h>

#include <QWidget>

namespace Ui
{
class KiranCpanelAppearance;
}

enum pages
{
    PAGE_BACKGROUND = 0,
    PAGE_THEME,
    PAGE_FONT,
    PAGE_BACKGROND_SELECT,
    PAGE_ICON_THEME,
    PAGE_CURSOR_THEME
};

enum items
{
    ITEM_BACKGROUND = 0,
    ITEM_THEME,
    ITEM_FONT
};

class QListWidgetItem;
class KiranCpanelAppearance : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit KiranCpanelAppearance(QWidget *parent = 0);
    ~KiranCpanelAppearance();

private:
    bool initUI();
    void addSidebarItem(QString, QString);

private:
    Ui::KiranCpanelAppearance *ui;
};

#endif  // KIRANCPANELAPPEARANCE_H
