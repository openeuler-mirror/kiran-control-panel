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

#ifndef ADD_IM_DIALOG_H
#define ADD_IM_DIALOG_H

#include <kiran-titlebar-window.h>

#include <QList>
#include <QSet>
#include <QString>
#include <QVector>

#include "im-candidate-filter.h"
#include "im-entry.h"
#include "im-list-provider.h"

class QLabel;
class QVBoxLayout;
class QPushButton;
class QScrollArea;
class QTimer;
class KiranSearchBox;
class ImChooseItem;

/// 添加输入法对话框
/// 顶部搜索框 + 中部可滚动候选列表（多选）+ 底部确定/取消
/// 已启用的输入法在列表中置灰且强制勾选，防止重复添加
class AddImDialog : public KiranTitlebarWindow
{
    Q_OBJECT
public:
    explicit AddImDialog(QWidget* parent = nullptr);
    ~AddImDialog() override;

    /// 设置候选过滤策略，需在 setProvider() 之前调用
    void setCandidateFilter(const ImCandidateFilter& filter);

    /// 设置数据源并加载候选列表（不持有所有权）
    /// 只要求提供 enabled/available 两组列表，不感知具体后端框架。
    void setProvider(ImListProvider* provider);

    /// 当前勾选且尚未启用的候选项
    QVector<ImEntry> selectedEntries() const;

signals:
    /// 点击确定，参数为需要新增的候选项（已排除已启用项）
    void entriesSelected(const QVector<ImEntry>& entries);

private:
    void initUI();
    void initConnection();
    /// 依据 provider 重新构建候选列表
    void reloadEntries();
    void clearItems();
    /// 按关键字过滤显示
    void applyFilter(const QString& keyword);
    /// 刷新确定按钮可用状态与空状态提示
    void updateState();

private slots:
    void onSearchTextChanged(const QString& text);
    void onConfirmClicked();

private:
    /// UI
    KiranSearchBox* m_searchBox = nullptr;
    QScrollArea* m_scrollArea = nullptr;
    QWidget* m_itemContainer = nullptr;
    QVBoxLayout* m_itemLayout = nullptr;
    QLabel* m_emptyLabel = nullptr;
    QPushButton* m_confirmButton = nullptr;
    QPushButton* m_cancelButton = nullptr;
    /// 搜索防抖，避免每次按键都遍历 700+ 项
    QTimer* m_searchTimer = nullptr;
    ImListProvider* m_provider = nullptr;
    /// 候选列表项
    QList<ImChooseItem*> m_items;
    /// 已启用项名称集合
    QSet<QString> m_enabledNames;
    ImCandidateFilter m_candidateFilter;
    /// 被过滤策略排除的候选数，用于区分空状态提示
    int m_filteredOutCount = 0;
};

#endif  // ADD_IM_DIALOG_H
