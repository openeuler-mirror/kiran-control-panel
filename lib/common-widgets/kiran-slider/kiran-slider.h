/**
 * Copyright (c) 2020 ~ 2023 KylinSec Co., Ltd.
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

#pragma once

#include <QAbstractSlider>

// TOOD:暂时只考虑水平滑块，并且标尺位于下方
class KiranSlider : public QAbstractSlider
{
    Q_OBJECT
public:
    KiranSlider(QWidget* parent = nullptr);
    ~KiranSlider();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    // 不同于QSlider中的标尺功能
    // 使用者根据使用场景，对指定点标记
    struct MarkPoint
    {
        MarkPoint(int value_, QString desc_);
        int value;
        QString desc;
    };
    void addMarks(QList<MarkPoint> marks);
    void addMark(MarkPoint mark, bool needUpdateUI = true);
    void deleteMark(int markValue);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* ev) override;
    void mouseReleaseEvent(QMouseEvent* ev) override;
    void mouseMoveEvent(QMouseEvent* ev) override;

private slots:
    void ensureLayoutUpdated();

private:
    struct UpdateContext
    {
        bool operator==(const UpdateContext& other) const;
        UpdateContext& operator=(const UpdateContext& other) = default;
        QSize size;
        int max;
        int min;
        int value;
    };
    UpdateContext getCurrentUpdateContext();
    // 根据具体值获取在滑动条中具体位置
    int sliderPositionFromValue(int min, int max,
                                int logicalValue, int span,
                                bool upsideDown);
    // 根据传入滑动条位置获取具体值
    int sliderValueFromPosition(int min, int max,
                                int pos, int span,
                                bool upsideDown);
    void drawBackground(QPainter& painter);
    void drawSliderGroove(QPainter& painter);
    void drawTickmarks(QPainter& painter);
    void drawSliderHandle(QPainter& painter);
    void drawTickmark(QPainter& painter, const MarkPoint& point);

private:
    QList<MarkPoint> m_markPoints;
    UpdateContext m_updateCache;

    QRect m_handleRect;
    QRect m_grooveRect;
    QRect m_contentRect;

    bool m_pressedHandle = false;
};