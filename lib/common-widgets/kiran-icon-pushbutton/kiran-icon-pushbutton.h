/**
 * Copyright (c) 2020 ~ 2022 KylinSec Co., Ltd.
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

#ifndef KIRAN_CONTROL_PANEL_KIRAN_ICON_PUSHBUTTON_H
#define KIRAN_CONTROL_PANEL_KIRAN_ICON_PUSHBUTTON_H

#include <QPushButton>
#include <style-palette.h>

/**
 * 对于普通的图标加入了一个图标颜色翻转跟随主题翻转的功能
 * TODO:该类只是为了暂时解决白色图标再浅色主题下不能看清的问题，后续需开发svg插件，使用kiranpalette进行渲染
 */
class KiranIconPushButton:public QPushButton
{
    Q_OBJECT
public:
    explicit KiranIconPushButton(QWidget* parent = nullptr);
    ~KiranIconPushButton();

    void setThemeIcon(QPixmap pixmap,Kiran::PaletteType paletteType);

private slots:
    void updatePixmapForTheme();

private:
    Kiran::PaletteType m_iconPaletteType=Kiran::PALETTE_LAST;
    QPixmap m_pixmap;
};

#endif  // KIRAN_CONTROL_PANEL_KIRAN_ICON_PUSHBUTTON_H
