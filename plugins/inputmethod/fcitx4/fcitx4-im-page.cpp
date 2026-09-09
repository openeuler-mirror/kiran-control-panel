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

#include "fcitx4-im-page.h"
#include "add-im-dialog.h"
#include "adjusting-scroll-area/adjusting-scroll-area.h"
#include "fcitx4/fcitx4-controller.h"
#include "im-key.h"
#include "key-sequence-edit/key-sequence-edit.h"
#include "logging-category.h"
#include "ui_fcitx4-im-page.h"
#include "widgets/im-list-view.h"

#include <kiran-message-box.h>
#include <kiran-push-button.h>
#include <style-property.h>

#include <QColor>
#include <QComboBox>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QPixmap>
#include <QPushButton>
#include <QSet>
#include <QSignalBlocker>
#include <QVBoxLayout>

using namespace Kiran;

namespace
{
// ShareInputState 协议枚举值（显示名由 createEnumComboBox 本地化）
const QString kEnumAll = QStringLiteral("All");
const QString kEnumProgram = QStringLiteral("Program");
const QString kEnumNo = QStringLiteral("No");

const int kSpacingBetweenGroups = 16;
const int kSpacingInGroup = 10;
const int kImItemHeight = 40;
const int kImItemSpacing = 8;
const int kImItemIconSize = 32;
const int kMaxVisibleImItemCount = 3;
}  // namespace

Fcitx4ImPage::Fcitx4ImPage(QWidget* parent)
    : Fcitx4ImPage(ImCandidateFilter(), parent)
{
}

Fcitx4ImPage::Fcitx4ImPage(const ImCandidateFilter& candidateFilter, QWidget* parent)
    : QWidget(parent),
      ui(new Ui::Fcitx4ImPage),
      m_controller(new Fcitx4Controller()),
      m_candidateFilter(candidateFilter)
{
    ui->setupUi(this);

    initUI();
    initConnection();
    loadEntries();
    loadSettings();
}

Fcitx4ImPage::~Fcitx4ImPage()
{
    delete m_controller;
    m_controller = nullptr;
    delete ui;
}

void Fcitx4ImPage::initUI()
{
    ui->stackedWidget->setCurrentWidget(ui->page_main);

    auto* containerLayout = new QVBoxLayout(ui->widget_imList);
    m_listContainerLayout = containerLayout;
    containerLayout->setContentsMargins(0, 0, 0, 0);
    // 标题 ↔ 列表为组内 10px；列表 ↔ 添加按钮为条目间 8px，逐段显式给出
    containerLayout->setSpacing(0);

    // 与设置区「快捷键」「行为」使用同一分组标题样式
    containerLayout->addWidget(createGroupTitle(tr("Input Method List")));
    containerLayout->addSpacing(kSpacingInGroup);

    // 列表语义说明：固定显示在滚动区外（滚动时保持可见）。
    // 首项为非激活态（通常是键盘布局），第二项为激活后默认输入法。
    auto* listHintLabel = new QLabel(
        tr("The first input method is the inactive state and should usually be a keyboard layout; "
           "the second is the default input method after activation."),
        ui->widget_imList);
    listHintLabel->setObjectName("label_hint");
    listHintLabel->setWordWrap(true);
    QPalette hintPalette = listHintLabel->palette();
    QColor hintColor = hintPalette.color(QPalette::WindowText);
    hintColor.setAlphaF(0.55);
    hintPalette.setColor(QPalette::WindowText, hintColor);
    listHintLabel->setPalette(hintPalette);
    QFont hintFont = listHintLabel->font();
    hintFont.setPointSizeF(qMax(6.0, hintFont.pointSizeF() - 1));
    listHintLabel->setFont(hintFont);
    containerLayout->addWidget(listHintLabel);
    containerLayout->addSpacing(kSpacingInGroup);

    // 内容增删会经 LayoutRequest 自动更新 sizeHint；不固定高度，也不调整窗口。
    m_listScrollArea = new AdjustingScrollArea(ui->widget_imList);
    m_listScrollArea->setFrameShape(QFrame::NoFrame);
    m_listScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_listView = new ImListView(m_listScrollArea->contentWidget());
    // 列表项与右侧（可能的垂直滚动条）之间保留间隙，避免内容紧贴滚动条。
    m_listScrollArea->contentLayout()->setContentsMargins(0, 0, kImItemSpacing, 0);
    m_listScrollArea->contentLayout()->addWidget(m_listView);
    // 只使用输入法分组已有的剩余空间；内容增加不会抬高父布局的 sizeHint。
    containerLayout->addWidget(m_listScrollArea, 1);
    containerLayout->addSpacing(kImItemSpacing);

    auto* addButton = new KiranPushButton(QString(), ui->widget_imList);
    StylePropertyHelper::setButtonType(addButton, BUTTON_Default);
    addButton->setFixedHeight(kImItemHeight);
    addButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    addButton->setIcon(QPixmap(":/kcp-inputmethod/images/addition.svg"));
    addButton->setIconSize(QSize(kImItemIconSize, kImItemIconSize));
    m_addButton = addButton;
    containerLayout->addWidget(m_addButton);

    initSettingsUI();

    // 设置区固定其自然高度（不被压缩）——空间不足时优先压缩列表滚动区
    // （内部滚动），避免设置区（快捷键/行为/高级）被挤压到不可见。
    // 这样默认窗口下滚动区拿到剩余空间，窗口最大化时滚动区随可用空间展开。
    ui->widget_settings->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    // 页面底部 spacer 吸收额外空间，使全部分组始终从顶部开始排列。
    ui->verticalLayout_main->setStretch(0, 0);
    ui->verticalLayout_main->setStretch(1, 0);
    ui->verticalLayout_main->setStretch(2, 1);

    // 列表滚动区最大高度固定为能容纳 3 个输入法项：输入法多于 3 项时
    // 在固定高度内滚动，不再按条目数把页面/窗口撑高。
    const int maxImContentHeight =
        kMaxVisibleImItemCount * kImItemHeight + (kMaxVisibleImItemCount - 1) * kImItemSpacing;
    m_listScrollArea->setMaximumContentHeight(maxImContentHeight);
}

QLabel* Fcitx4ImPage::createGroupTitle(const QString& text)
{
    auto* title = new QLabel(text);
    QFont titleFont = title->font();
    titleFont.setBold(true);
    title->setFont(titleFont);
    return title;
}

QWidget* Fcitx4ImPage::createSettingRow(const QString& text, QWidget* control)
{
    auto* row = new QWidget();
    auto* layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(16);

    auto* label = new QLabel(text, row);
    layout->addWidget(label);
    layout->addStretch();

    control->setParent(row);
    layout->addWidget(control);

    return row;
}

QComboBox* Fcitx4ImPage::createEnumComboBox()
{
    auto* combo = new QComboBox();
    combo->addItem(tr("All programs"), kEnumAll);
    combo->addItem(tr("By program"), kEnumProgram);
    combo->addItem(tr("Off"), kEnumNo);
    combo->setMinimumWidth(140);
    return combo;
}

QComboBox* Fcitx4ImPage::createImSwitchBackwardComboBox()
{
    auto* combo = new QComboBox();
    // IMSwitchHotkey：CTRL_SHIFT / ALT_SHIFT / CTRL_SUPER / ALT_SUPER
    combo->addItem(tr("Ctrl + Left Shift"), QStringLiteral("CTRL_SHIFT"));
    combo->addItem(tr("Alt + Left Shift"), QStringLiteral("ALT_SHIFT"));
    combo->addItem(tr("Ctrl + Left Super"), QStringLiteral("CTRL_SUPER"));
    combo->addItem(tr("Alt + Left Super"), QStringLiteral("ALT_SUPER"));
    combo->setMinimumWidth(140);
    return combo;
}

void Fcitx4ImPage::initSettingsUI()
{
    auto* layout = new QVBoxLayout(ui->widget_settings);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(kSpacingBetweenGroups);

    // ---- 快捷键 ----
    auto* shortcutsGroup = new QWidget(ui->widget_settings);
    auto* shortcutsLayout = new QVBoxLayout(shortcutsGroup);
    shortcutsLayout->setContentsMargins(0, 0, 0, 0);
    shortcutsLayout->setSpacing(kSpacingInGroup);

    shortcutsLayout->addWidget(createGroupTitle(tr("Shortcuts")));

    m_triggerKeyEdit = new KeySequenceEdit();
    m_triggerKeyEdit->setFormatter(m_controller->createKeyFormatter());
    shortcutsLayout->addWidget(
        createSettingRow(tr("Switch activation/inactivation of input method"), m_triggerKeyEdit));

    m_imSwitchBackwardCombo = createImSwitchBackwardComboBox();
    shortcutsLayout->addWidget(
        createSettingRow(tr("Switch to previous input method"), m_imSwitchBackwardCombo));

    layout->addWidget(shortcutsGroup);

    // ---- 行为 ----
    auto* behaviorGroup = new QWidget(ui->widget_settings);
    auto* behaviorLayout = new QVBoxLayout(behaviorGroup);
    behaviorLayout->setContentsMargins(0, 0, 0, 0);
    behaviorLayout->setSpacing(kSpacingInGroup);

    behaviorLayout->addWidget(createGroupTitle(tr("Behavior")));

    m_shareInputStateCombo = createEnumComboBox();
    behaviorLayout->addWidget(createSettingRow(tr("Share input state"), m_shareInputStateCombo));

    layout->addWidget(behaviorGroup);

    // ---- 高级设置 ----
    auto* advancedGroup = new QWidget(ui->widget_settings);
    auto* advancedLayout = new QVBoxLayout(advancedGroup);
    advancedLayout->setContentsMargins(0, 0, 0, 0);
    advancedLayout->setSpacing(kSpacingInGroup);

    auto* advancedButton = new KiranPushButton(tr("Advanced Settings..."));
    auto* advancedRow = new QHBoxLayout();
    advancedRow->setContentsMargins(0, 0, 0, 0);
    advancedRow->addWidget(advancedButton);
    advancedRow->addStretch();
    advancedLayout->addLayout(advancedRow);

    auto* advancedTips = new QLabel(
        tr("More detailed input method options are available in the input method configuration tool."));
    advancedTips->setWordWrap(true);
    QPalette tipsPalette = advancedTips->palette();
    QColor tipsColor = tipsPalette.color(QPalette::WindowText);
    tipsColor.setAlphaF(0.55);
    tipsPalette.setColor(QPalette::WindowText, tipsColor);
    advancedTips->setPalette(tipsPalette);
    advancedLayout->addWidget(advancedTips);

    layout->addWidget(advancedGroup);

    connect(advancedButton, &QPushButton::clicked, this, &Fcitx4ImPage::onAdvancedSettingsClicked);
}

void Fcitx4ImPage::initConnection()
{
    connect(m_listView, &ImListView::orderChanged, this, &Fcitx4ImPage::onOrderChanged);
    connect(m_listView, &ImListView::entryRemoved, this, &Fcitx4ImPage::onEntryRemoved);
    connect(m_addButton, &QPushButton::clicked, this, &Fcitx4ImPage::onAddRequested);

    connect(m_triggerKeyEdit, &KeySequenceEdit::keyEntryChanged,
            this, &Fcitx4ImPage::onTriggerKeyChanged);
    connect(m_triggerKeyEdit, &KeySequenceEdit::keyEntryCleared,
            this, &Fcitx4ImPage::onTriggerKeyCleared);
    connect(m_shareInputStateCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &Fcitx4ImPage::onShareInputStateChanged);
    connect(m_imSwitchBackwardCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &Fcitx4ImPage::onImSwitchBackwardChanged);

    connect(m_controller, &Fcitx4Controller::settingsChanged,
            this, &Fcitx4ImPage::reloadFromExternal);
    connect(m_controller, &Fcitx4Controller::availabilityChanged,
            this, &Fcitx4ImPage::onAvailabilityChanged);
}

void Fcitx4ImPage::reloadFromExternal()
{
    if (!m_controller || !m_controller->isAvailable())
    {
        return;
    }

    loadEntries();   // 内部有幂等短路，数据未变时不重建
    loadSettings();  // 全程 QSignalBlocker 包裹，回填不会触发回写
}

void Fcitx4ImPage::onAvailabilityChanged(bool available)
{
    if (available)
    {
        // 服务恢复：全量重读（列表 + 设置），设置区恢复可用
        reloadFromExternal();
    }
    else
    {
        // 服务退出：设置区禁用、列表清空（与 loadSettings/loadEntries 空态一致）
        ui->widget_settings->setEnabled(false);
        m_listView->setEntries({});
    }
}

void Fcitx4ImPage::loadSettings()
{
    if (!m_controller || !m_controller->isAvailable())
    {
        qCWarning(qLcInputMethod) << "fcitx4 isn't available, disable settings area";
        ui->widget_settings->setEnabled(false);
        return;
    }

    ui->widget_settings->setEnabled(true);

    {
        QSignalBlocker triggerBlocker(m_triggerKeyEdit);
        QSignalBlocker shareBlocker(m_shareInputStateCombo);
        QSignalBlocker imSwitchBlocker(m_imSwitchBackwardCombo);

        const KeyEntryList triggerKeys = m_controller->keyList(ImKey::triggerKeys);
        if (triggerKeys.isEmpty())
            m_triggerKeyEdit->clearKeyEntry();
        else
            m_triggerKeyEdit->setKeyEntry(triggerKeys.first());

        const QString shareState = m_controller->globalEnum(ImKey::shareInputState);
        int shareIndex = m_shareInputStateCombo->findData(shareState);
        m_shareInputStateCombo->setCurrentIndex(shareIndex >= 0 ? shareIndex : 0);
        if (shareIndex < 0 && !shareState.isEmpty())
            qCWarning(qLcInputMethod) << "unknown ShareInputState value:" << shareState;

        const QString imSwitch = m_controller->globalEnum(ImKey::imSwitchBackwardKey);
        int imIndex = m_imSwitchBackwardCombo->findData(imSwitch);
        m_imSwitchBackwardCombo->setCurrentIndex(imIndex >= 0 ? imIndex : 0);
        if (imIndex < 0 && !imSwitch.isEmpty())
            qCWarning(qLcInputMethod) << "unknown imSwitchBackwardKey value:" << imSwitch;
    }
}

void Fcitx4ImPage::writeKeySequence(const QString& key, bool hasEntry, const KeyEntry& entry)
{
    if (!m_controller)
        return;

    // setKeyList 全量覆盖；界面只改第 0 项，须先读出再写，保留第 1 项（fcitx4 最多 2 组）
    KeyEntryList keys = m_controller->keyList(key);

    if (!hasEntry)
    {
        // 清空第 0 项：列表随之左移；原本只有一组则变成空列表（禁用该热键）
        if (keys.isEmpty())
            return;
        keys.removeFirst();
    }
    else if (keys.isEmpty())
    {
        keys.append(entry);
    }
    else if (keys.first() == entry)
    {
        return;  // 与当前第 0 项相同，跳过写盘
    }
    else
    {
        keys[0] = entry;
    }

    if (!m_controller->setKeyList(key, keys))
    {
        qCWarning(qLcInputMethod) << "failed to write key list" << key;
        showConfigWriteError();
        loadSettings();
    }
}

void Fcitx4ImPage::showConfigWriteError()
{
    KiranMessageBox::message(this, tr("Error"), tr("Failed to save input method settings."),
                             KiranMessageBox::Ok);
}

void Fcitx4ImPage::onTriggerKeyChanged(const KeyEntry& entry)
{
    writeKeySequence(ImKey::triggerKeys, true, entry);
}

void Fcitx4ImPage::onTriggerKeyCleared()
{
    writeKeySequence(ImKey::triggerKeys, false);
}

void Fcitx4ImPage::onShareInputStateChanged(int index)
{
    if (!m_controller || index < 0)
    {
        return;
    }
    const QString value = m_shareInputStateCombo->itemData(index).toString();
    if (!m_controller->setGlobalEnum(ImKey::shareInputState, value))
    {
        qCWarning(qLcInputMethod) << "failed to write" << ImKey::shareInputState << value;
        showConfigWriteError();
        loadSettings();
    }
}

void Fcitx4ImPage::onImSwitchBackwardChanged(int index)
{
    if (!m_controller || index < 0)
    {
        return;
    }
    const QString value = m_imSwitchBackwardCombo->itemData(index).toString();
    if (!m_controller->setGlobalEnum(ImKey::imSwitchBackwardKey, value))
    {
        qCWarning(qLcInputMethod) << "failed to write" << ImKey::imSwitchBackwardKey << value;
        showConfigWriteError();
        loadSettings();
    }
}

void Fcitx4ImPage::onAdvancedSettingsClicked()
{
    if (!m_controller)
    {
        return;
    }
    m_controller->openAdvancedSettings();
}

bool Fcitx4ImPage::isSameEntryOrder(const QVector<ImEntry>& lhs, const QVector<ImEntry>& rhs)
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }

    for (int i = 0; i < lhs.size(); i++)
    {
        if (lhs.at(i).uniqueName != rhs.at(i).uniqueName)
        {
            return false;
        }
    }
    return true;
}

void Fcitx4ImPage::loadEntries()
{
    if (!m_controller || !m_controller->isAvailable())
    {
        qCWarning(qLcInputMethod) << "fcitx4 isn't available, show empty list";
        m_listView->setEntries({});
        return;
    }

    const QVector<ImEntry> entries = m_controller->enabledEntries();

    // 数据未变则不重建：避免页面自身写入经变更通知回环导致的无谓重建，
    // 也避免打断用户的 hover / 滚动
    if (isSameEntryOrder(m_listView->entries(), entries))
    {
        return;
    }

    m_listView->setEntries(entries);
}

void Fcitx4ImPage::onOrderChanged(const QVector<ImEntry>& entries)
{
    if (!m_controller)
    {
        return;
    }

    if (!m_controller->setEnabledEntries(entries))
    {
        qCWarning(qLcInputMethod) << "failed to write reordered input method list, reload from fcitx4";
        showConfigWriteError();
    }

    // 无论成功与否都以 fcitx4 实际状态为准（默认输入法由后端按新列表第二位决定）
    loadEntries();
}

void Fcitx4ImPage::onEntryRemoved(const QString& uniqueName)
{
    if (!m_controller)
    {
        return;
    }

    // 列表控件已完成本地移除，此处按其当前内容写回
    if (!m_controller->setEnabledEntries(m_listView->entries()))
    {
        qCWarning(qLcInputMethod) << "failed to remove input method:" << uniqueName;
        showConfigWriteError();
    }

    // 重新读取：删除默认输入法后，后端会将默认项定位到新列表的第二项
    loadEntries();
}

void Fcitx4ImPage::onAddRequested()
{
    if (!m_controller || !m_controller->isAvailable())
    {
        qCWarning(qLcInputMethod) << "fcitx4 isn't available, ignore add request";
        return;
    }

    auto* dialog = new AddImDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setCandidateFilter(m_candidateFilter);
    dialog->setProvider(m_controller);

    connect(dialog, &AddImDialog::entriesSelected, this, &Fcitx4ImPage::onEntriesSelected);

    dialog->show();
}

void Fcitx4ImPage::onEntriesSelected(const QVector<ImEntry>& entries)
{
    if (!m_controller || entries.isEmpty())
    {
        return;
    }

    // 以 fcitx4 当前状态为基准追加，避免与对话框打开期间的外部改动冲突
    QVector<ImEntry> merged = m_controller->enabledEntries();

    QSet<QString> existing;
    for (const auto& entry : merged)
    {
        existing.insert(entry.uniqueName);
    }

    int appended = 0;
    for (const auto& entry : entries)
    {
        // 二次去重：对话框已置灰已启用项，这里防御并发改动
        if (existing.contains(entry.uniqueName))
        {
            qCDebug(qLcInputMethod) << "skip already enabled input method:" << entry.uniqueName;
            continue;
        }
        merged.append(entry);
        existing.insert(entry.uniqueName);
        appended++;
    }

    if (appended == 0)
    {
        return;
    }

    if (!m_controller->setEnabledEntries(merged))
    {
        qCWarning(qLcInputMethod) << "failed to add" << appended << "input method(s)";
        showConfigWriteError();
    }

    loadEntries();
}

QSize Fcitx4ImPage::sizeHint() const
{
    return {419, 595};
}
