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

#ifndef FCITX4_IM_PAGE_H
#define FCITX4_IM_PAGE_H

#include <QVector>
#include <QWidget>

#include "im-candidate-filter.h"
#include "im-entry.h"
#include "key-sequence-edit/key-formatter.h"

namespace Ui
{
class Fcitx4ImPage;
}

class QComboBox;
class QLabel;
class QVBoxLayout;
class KiranPushButton;
class Fcitx4Controller;
class ImListView;
class AdjustingScrollArea;
class AddImDialog;
class KeySequenceEdit;

class Fcitx4ImPage : public QWidget
{
    Q_OBJECT
public:
    explicit Fcitx4ImPage(QWidget* parent = nullptr);
    explicit Fcitx4ImPage(const ImCandidateFilter& candidateFilter, QWidget* parent = nullptr);
    ~Fcitx4ImPage();

    QSize sizeHint() const override;

private:
    void initUI();
    /// 构建「快捷键」「行为」「高级设置」三块设置区
    void initSettingsUI();
    void initConnection();
    /// 从 fcitx4 加载已启用列表并刷新界面
    void loadEntries();
    /// 从 fcitx4 加载快捷键与行为设置（阻塞信号，避免回填触发写回）
    void loadSettings();
    /// 组装一行「左标签 + 右控件」
    QWidget* createSettingRow(const QString& text, QWidget* control);
    /// 分组标题
    QLabel* createGroupTitle(const QString& text);
    /// 共享状态三态下拉（All / Program / No）
    QComboBox* createEnumComboBox();
    /// 「向后切换输入法」下拉（IMSwitchHotkey 四预设）
    QComboBox* createImSwitchBackwardComboBox();
    /// 写回快捷键：只改第 0 项，保留其余组（setKeyList 为全量覆盖）
    void writeKeySequence(const QString& key, bool hasEntry, const KeyEntry& entry = KeyEntry());
    void showConfigWriteError();
    /// 只比较 uniqueName 序列（显示名/图标为静态元数据；默认项恒为第二项）
    static bool isSameEntryOrder(const QVector<ImEntry>& lhs, const QVector<ImEntry>& rhs);

private slots:
    void onOrderChanged(const QVector<ImEntry>& entries);
    void onEntryRemoved(const QString& uniqueName);
    void onAddRequested();
    void onEntriesSelected(const QVector<ImEntry>& entries);
    void reloadFromExternal();
    void onAvailabilityChanged(bool available);
    void onTriggerKeyChanged(const KeyEntry& entry);
    void onTriggerKeyCleared();
    void onShareInputStateChanged(int index);
    void onImSwitchBackwardChanged(int index);
    void onAdvancedSettingsClicked();

private:
    Ui::Fcitx4ImPage* ui;
    Fcitx4Controller* m_controller = nullptr;
    ImListView* m_listView = nullptr;
    AdjustingScrollArea* m_listScrollArea = nullptr;
    QVBoxLayout* m_listContainerLayout = nullptr;
    KiranPushButton* m_addButton = nullptr;
    ImCandidateFilter m_candidateFilter;

    KeySequenceEdit* m_triggerKeyEdit = nullptr;
    QComboBox* m_shareInputStateCombo = nullptr;
    QComboBox* m_imSwitchBackwardCombo = nullptr;
};

#endif
