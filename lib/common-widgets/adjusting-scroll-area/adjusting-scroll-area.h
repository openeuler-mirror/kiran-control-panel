/**
 * Copyright (c) 2020 ~ 2026 KylinSec Co., Ltd.
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

#ifndef ADJUSTING_SCROLL_AREA_H
#define ADJUSTING_SCROLL_AREA_H

#include <QScrollArea>

class QVBoxLayout;

/**
 * 期望高度跟随内部垂直布局自适应的滚动区。
 *
 * maximumContentHeight 为外框高度上限：包含边框与已启用的滚动条，
 * 非正值表示不设上限。垂直 SizePolicy 默认为 Minimum，父级布局可将
 * 其压缩为可视视口的高度，而不会随内容高度一起膨胀。
 */
class AdjustingScrollArea : public QScrollArea
{
    Q_OBJECT
    Q_PROPERTY(int maximumContentHeight READ maximumContentHeight WRITE setMaximumContentHeight)

public:
    explicit AdjustingScrollArea(QWidget* parent = nullptr);

    QWidget* contentWidget() const;
    QVBoxLayout* contentLayout() const;

    int maximumContentHeight() const;
    void setMaximumContentHeight(int height);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void scheduleGeometryUpdate();
    /// 判断事件是否表示内容几何需要重算，用于把 eventFilter 中的关注事件
    /// 判定逻辑剥离为可读的小函数。
    bool isContentGeometryChange(QObject* watched, QEvent* event) const;

    QWidget* m_contentWidget = nullptr;
    QVBoxLayout* m_contentLayout = nullptr;
    int m_maximumContentHeight = 0;
    /// 「updateGeometry 是否已预约但尚未执行」标志。
    ///
    /// 内容 widget / viewport 上的 LayoutRequest 与 Resize 事件可能在一拍
    /// 内多次到达（典型场景：连续 addWidget 多个条目），该标志在第一次
    /// 预约时置位、回调里清除，确保整拍只触发一次 updateGeometry；
    /// 回调先清标志再做工作，是为了让工作过程中若再引发相关事件仍可
    /// 正常预约下一轮。
    bool m_geometryUpdatePending = false;
};

#endif  // ADJUSTING_SCROLL_AREA_H
