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

#include "add-im-dialog.h"
#include "widgets/im-choose-item.h"
#include "logging-category.h"

#include <kiran-push-button.h>
#include <kiran-search-box.h>
#include <style-property.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>

using namespace Kiran;

namespace
{
const int kDialogWidth = 500;
const int kDialogHeight = 500;
const int kItemSpacing = 2;
// 搜索防抖间隔
const int kSearchDelayMs = 150;
}  // namespace

AddImDialog::AddImDialog(QWidget* parent)
    : KiranTitlebarWindow(parent, Qt::Dialog)
{
    initUI();
    initConnection();
}

AddImDialog::~AddImDialog()
{
}

void AddImDialog::initUI()
{
    setTitle(tr("Add Input Method"));
    setWindowModality(Qt::ApplicationModal);
    setButtonHints(TitlebarCloseButtonHint);
    setTitlebarColorBlockEnable(true);
    resize(kDialogWidth, kDialogHeight);

    auto* content = getWindowContentWidget();
    auto* mainLayout = new QVBoxLayout(content);
    mainLayout->setContentsMargins(24, 16, 24, 24);
    mainLayout->setSpacing(16);

    // 搜索框
    m_searchBox = new KiranSearchBox(content);
    m_searchBox->setObjectName("lineEdit_search");
    m_searchBox->setPlaceholderText(tr("Search input method"));
    m_searchBox->setClearButtonEnabled(true);
    mainLayout->addWidget(m_searchBox);

    // 候选列表滚动区
    m_scrollArea = new QScrollArea(content);
    m_scrollArea->setObjectName("scrollArea_candidates");
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->viewport()->setAutoFillBackground(false);

    m_itemContainer = new QWidget(m_scrollArea);
    m_itemContainer->setObjectName("widget_candidates");
    m_itemContainer->setAutoFillBackground(false);
    m_itemLayout = new QVBoxLayout(m_itemContainer);
    m_itemLayout->setContentsMargins(0, 0, 0, 0);
    m_itemLayout->setSpacing(kItemSpacing);
    // 条目顶部对齐，数量少时不要被拉伸
    m_itemLayout->addStretch();

    m_scrollArea->setWidget(m_itemContainer);
    mainLayout->addWidget(m_scrollArea, 1);

    // 空状态提示（无候选 / 搜索无结果）
    m_emptyLabel = new QLabel(content);
    m_emptyLabel->setObjectName("label_empty");
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->hide();
    mainLayout->addWidget(m_emptyLabel);

    // 底部按钮
    m_confirmButton = new KiranPushButton(tr("OK"), content);
    StylePropertyHelper::setButtonType(m_confirmButton, BUTTON_Default);
    m_confirmButton->setEnabled(false);

    m_cancelButton = new KiranPushButton(tr("Cancel"), content);

    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(16);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_confirmButton);
    mainLayout->addLayout(buttonLayout);

    m_searchTimer = new QTimer(this);
    m_searchTimer->setSingleShot(true);
    m_searchTimer->setInterval(kSearchDelayMs);
}

void AddImDialog::initConnection()
{
    connect(m_searchBox, &QLineEdit::textChanged, this, &AddImDialog::onSearchTextChanged);
    connect(m_searchTimer, &QTimer::timeout, this,
            [this]
            {
                applyFilter(m_searchBox->text().trimmed());
            });

    connect(m_confirmButton, &QPushButton::clicked, this, &AddImDialog::onConfirmClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &AddImDialog::close);
}

void AddImDialog::setCandidateFilter(const ImCandidateFilter& filter)
{
    m_candidateFilter = filter;
}

void AddImDialog::setProvider(ImListProvider* provider)
{
    m_provider = provider;
    reloadEntries();
}

void AddImDialog::clearItems()
{
    for (auto* item : m_items)
    {
        m_itemLayout->removeWidget(item);
        item->deleteLater();
    }
    m_items.clear();
}

void AddImDialog::reloadEntries()
{
    clearItems();
    m_enabledNames.clear();
    m_filteredOutCount = 0;

    if (!m_provider || !m_provider->isAvailable())
    {
        qCWarning(qLcInputMethod) << "input method provider isn't available, no candidate to add";
        updateState();
        return;
    }

    // 先记录已启用项，用于置灰去重
    const QVector<ImEntry> enabled = m_provider->enabledEntries();
    for (const auto& entry : enabled)
    {
        m_enabledNames.insert(entry.uniqueName);
    }

    const QVector<ImEntry> available = m_provider->availableEntries();
    for (const auto& entry : available)
    {
        // 在建条目之前过滤：后端可能返回大量候选（绝大多数是键盘布局），
        // 全量建 widget 再隐藏既浪费又让搜索能命中本应排除的项。
        if (!m_candidateFilter.accepts(entry))
        {
            m_filteredOutCount++;
            continue;
        }

        auto* item = new ImChooseItem(m_itemContainer);
        item->setEntry(entry);
        item->setAlreadyEnabled(m_enabledNames.contains(entry.uniqueName));

        connect(item, &ImChooseItem::toggled, this,
                [this](bool)
                {
                    updateState();
                });

        // 插到 stretch 之前，保证条目始终顶部对齐
        m_itemLayout->insertWidget(m_itemLayout->count() - 1, item);
        m_items.append(item);
    }

    qCDebug(qLcInputMethod) << "loaded" << m_items.size() << "candidates,"
                            << m_enabledNames.size() << "already enabled,"
                            << m_filteredOutCount << "filtered out"
                            << "(allowKeyboardLayout" << m_candidateFilter.allowKeyboardLayout()
                            << "onlyCurrentLanguage" << m_candidateFilter.onlyCurrentLanguage() << ")";

    applyFilter(m_searchBox->text().trimmed());
}

void AddImDialog::applyFilter(const QString& keyword)
{
    for (auto* item : m_items)
    {
        item->setVisible(item->matches(keyword));
    }

    // 过滤后回到顶部，否则可能停留在空白区域
    if (m_scrollArea->verticalScrollBar())
    {
        m_scrollArea->verticalScrollBar()->setValue(0);
    }

    updateState();
}

void AddImDialog::updateState()
{
    int visibleCount = 0;
    for (auto* item : m_items)
    {
        if (!item->isHidden())
        {
            visibleCount++;
        }
    }

    // 空状态提示：区分「搜索无结果」「已无可添加项」「被配置过滤」
    bool showEmpty = (visibleCount == 0);
    if (showEmpty)
    {
        QString emptyText;
        if (!m_items.isEmpty())
        {
            emptyText = tr("No search results, please search again...");
        }
        else if (m_filteredOutCount > 0)
        {
            // 候选并非不存在，而是被过滤策略排除，避免误解为系统未安装输入法
            emptyText = tr("No input method available for the current language");
        }
        else
        {
            emptyText = tr("No input method available");
        }
        m_emptyLabel->setText(emptyText);
    }
    m_emptyLabel->setVisible(showEmpty);
    m_scrollArea->setVisible(!showEmpty);

    m_confirmButton->setEnabled(!selectedEntries().isEmpty());
}

QVector<ImEntry> AddImDialog::selectedEntries() const
{
    QVector<ImEntry> result;
    for (auto* item : m_items)
    {
        // 已启用项虽然是勾选态，但不属于「新增」
        if (item->isChecked() && !item->alreadyEnabled())
        {
            result.append(item->entry());
        }
    }
    return result;
}

void AddImDialog::onSearchTextChanged(const QString& text)
{
    Q_UNUSED(text);
    // 防抖：连续输入时只在停顿后过滤一次
    m_searchTimer->start();
}

void AddImDialog::onConfirmClicked()
{
    const QVector<ImEntry> selected = selectedEntries();
    if (selected.isEmpty())
    {
        return;
    }

    emit entriesSelected(selected);
    close();
}
