//
// Created by liuxinhao on 2022/6/17.
//

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
