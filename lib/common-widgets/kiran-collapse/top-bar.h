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
 * Author:     youzhengcai <youzhengcai@kylinsec.com.cn>
 */

#pragma once

#include <QWidget>

namespace Ui
{
class TopBar;
}

class TopBar : public QWidget
{
    Q_OBJECT

public:
    explicit TopBar(QWidget* parent = nullptr);
    ~TopBar() override;
    // 设置标题
    void setTitle(const QString& title);
    // 添加控件
    void addWidget(QWidget* widget);
    /**
     * @brief 设置扩展区内部间距
     * @param left, top, right, bottom
     */
    void setTopBarMargin(int left, int top, int right, int bottom);
    // 设置控件间距
    void setTopBarSpacing(int spacing);
    // 接收图标 URL
    void setFlagPixmap(const QString& flag_url);
    // 接收 Pixmap 图标
    void setFlagPixmap(const QPixmap& pixmap);
    // 刷新 Pixmap 图标
    void refreshFlagPixmap(bool isExpanded);

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    void clickedBar();

private:
    void init();

private:
    Ui::TopBar* ui;

    QString m_title;     // 顶栏标题
    int m_height = 45;   // 顶栏高度， 默认45
    int m_spacing = 12;  // 顶栏内部组件间间隔
    int m_topBarMarginLeft = 12;
    int m_topBarMarginTop = 5;
    int m_topBarMarginRight = 12;
    int m_topBarMarginBottom = 5;

    QPixmap m_flag;  // 顶栏折叠展开图标

    QPoint m_mousePos;  // 鼠标位置，用于自定义点击事件
};
