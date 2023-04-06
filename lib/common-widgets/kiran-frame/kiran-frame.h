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

#ifndef KIRAN_CONTROL_PANEL_KIRAN_FRAME_H
#define KIRAN_CONTROL_PANEL_KIRAN_FRAME_H

#include <QWidget>

#include <style-palette.h>

//给控制中心组件提供色块的控件,用于突出显示
class KiranFrame : public QWidget
{
    Q_OBJECT
public:
    explicit KiranFrame(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    /**
     * 设置圆角
     */
    void setRadius(int radius);
    /**
     * 获取圆角值
     */
    int getRadius();

    /**
     * 获取是否绘制背景
     */
    bool getDrawBackground();
    /**
     * 设置是否绘制背景
     */
    void setDrawBackground(bool enable);
    /**
     * 设置绘制背景固定的状态，不根据实际状态从KiranPalette之中取出颜色(除了disable状态，disable将还是从KiranPalette之中取出禁用色)
     * @param state 希望固定的颜色状态
     */
    void setFixedBackgroundState(Kiran::StylePalette::ColorState state);
    /**
     * 取消掉固定背景状态，背景色根据实际状态取出
     */
    void unsetFixedBackgroundState();

    /**
     * 获取是否绘制边框
     */
    bool getDrawBroder();
    /**
     * 设置是否绘制边框
     */
    void setDrawBroder(bool enable);
    
    /**
     * 设置边框宽度
    */
    void setBorderWidth(int width);

    /**
     * 设置绘制背景固定的状态，不根据实际状态从KiranPalette之中取出颜色(除了disable状态，disable将还是从KiranPalette之中取出禁用色)
     * @param state 希望固定的颜色状态
     */
    void setFixedBorderState(Kiran::StylePalette::ColorState state);
    /**
     * 取消掉固定背景状态，背景色根据实际状态取出
     */
    void unsetFixedBorderState();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int m_radius = 6;

    bool m_drawBackground = true;
    bool m_fixedBackground = false;
    Kiran::StylePalette::ColorState m_fixedBackgroundState = Kiran::StylePalette::Normal;

    bool m_drawBorder = true;
    int m_borderWidth = 1;
    bool m_fixedBorder = false;
    Kiran::StylePalette::ColorState m_fixedBorderState = Kiran::StylePalette::Normal;
};

#endif  // KIRAN_CONTROL_PANEL_KIRAN_FRAME_H
