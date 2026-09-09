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

#ifndef IM_CHOOSE_ITEM_H
#define IM_CHOOSE_ITEM_H

#include <QWidget>

#include "im-entry.h"

class QCheckBox;

/// 可选输入法条目（添加输入法对话框中使用）
/// 布局：复选框 | 图标(24x24) | 主名称 + 子名称(灰) | 语言标签
/// 已启用的输入法显示为「勾选且不可操作」，避免重复添加
class ImChooseItem : public QWidget
{
    Q_OBJECT
public:
    explicit ImChooseItem(QWidget* parent = nullptr);
    ~ImChooseItem() override;

    void setEntry(const ImEntry& entry);
    const ImEntry& entry() const { return m_entry; }
    QString uniqueName() const { return m_entry.uniqueName; }

    void setChecked(bool checked);
    bool isChecked() const;

    /// 标记为「已启用」：勾选并禁止取消，防止重复添加
    void setAlreadyEnabled(bool alreadyEnabled);
    bool alreadyEnabled() const { return m_alreadyEnabled; }

    /// 供搜索过滤使用的匹配串（uniqueName / name / nativeName / langCode）
    bool matches(const QString& keyword) const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void toggled(bool checked);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void initUI();
    QString primaryText() const;
    QString secondaryText() const;
    QRect langLabelRect() const;

private:
    ImEntry m_entry;
    QCheckBox* m_checkBox = nullptr;
    bool m_alreadyEnabled = false;
    bool m_hovered = false;
};

#endif  // IM_CHOOSE_ITEM_H
