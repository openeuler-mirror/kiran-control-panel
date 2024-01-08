/**
 * Copyright (c) 2022 ~ 2023 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yinhongchang <yinhongchang@kylinsec.com.cn>
 */

#include <kiran-search-box.h>
#include <QStandardItemModel>
#include <QCompleter>
#include <QLineEdit>
#include <QPushButton>
#include <QToolTip>
#include <QListView>
#include <qt5-log-i.h>
#include "language-select-dialog.h"
#include "ui_language-select-dialog.h"
#include "search-delegate.h"

LanguageSelectDialog::LanguageSelectDialog(QWidget* parent) : QDialog(parent),
                    ui(new Ui::LanguageSelectDialog), m_kiranSearchBox(new KiranSearchBox(this))
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Add Language"));
    m_kiranSearchBox->setPlaceholderText(tr("Search"));
    ui->frame->layout()->addWidget(m_kiranSearchBox);
    ui->tableView->setStyleSheet("QTableView{border: 1px solid palette(midlight);}");
    model = new QStandardItemModel(this);
    ui->tableView->setModel(model);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setMouseTracking(true);
    ui->tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    connect(ui->tableView, &QTableView::entered,
            this, [=](const QModelIndex &index){
        QString mStr = index.data().toString();
        QFontMetrics fontMetrics(this->font());
        int fontSize = fontMetrics.width(mStr);
        if (fontSize - 16 > 416) {
            QToolTip::showText(QCursor::pos(), mStr);
        }
    });
    // 隐藏垂直表头
    ui->tableView->verticalHeader()->setVisible(false);
    // 隐藏水平表头
    ui->tableView->horizontalHeader()->setVisible(false);
    // 隐藏水平滑动条
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 不显示表格线
    ui->tableView->setShowGrid(false);
    // 水平表格自动拉伸
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 行高默认为36
    ui->tableView->verticalHeader()->setDefaultSectionSize(36);
    this->setWindowModality(Qt::ApplicationModal); 
    // 不可编辑
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    model->setColumnCount(1);
    // 加载Completer到KiranSearchBox中，便于搜索
    initCompleter();

    // 选中语言添加
    connect(ui->yesBtn, &QPushButton::clicked, this, [=]() {
        QModelIndexList indexList = ui->tableView->selectionModel()->selectedRows();
        QString locale;
        if (!indexList.empty())
        {
            QModelIndex modelIndex = indexList.first();
            locale = model->data(modelIndex, Qt::UserRole + 1).toString();
        }
        emit languageAddBtnClicked(locale);
        this->hide();
    });
    // 取消
    connect(ui->noBtn, &QPushButton::clicked, this, [=]() {
        this->hide();
    });
}

LanguageSelectDialog::~LanguageSelectDialog()
{
}

void LanguageSelectDialog::initCompleter()
{
    m_searchDelegate = new SearchDelegate(this);
    m_completer = new QCompleter(this);
    m_completer->setModel(model);
    m_completer->setMaxVisibleItems(6);
    m_completer->popup()->setAttribute(Qt::WA_InputMethodEnabled);
    m_completer->popup()->setItemDelegate(m_searchDelegate);
    m_completer->setFilterMode(Qt::MatchContains);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setWrapAround(false);
    if (m_kiranSearchBox) {
        m_kiranSearchBox->setCompleter(m_completer);
    }

    connect(m_kiranSearchBox, &QLineEdit::returnPressed, this, [=]() {
        if (!m_kiranSearchBox->text().isEmpty()) {
            QList<QStandardItem*> itemList = model->findItems(m_kiranSearchBox->text());
            if (!itemList.isEmpty()) {
                QModelIndex m_modeIndex = model->indexFromItem(itemList.first());
                ui->tableView->scrollTo(m_modeIndex);
                int index = m_modeIndex.row();
                ui->tableView->selectRow(index);
            }
        }
    });

    connect(m_kiranSearchBox, &QLineEdit::textChanged, this, [=](QString text){
        for (int i = 0; i < model->rowCount(); i++) {
            if (text.contains(model->data(model->index(i,0)).toString())) {
                m_kiranSearchBox->blockSignals(true);
                m_kiranSearchBox->setText(model->item(i)->text());
                m_kiranSearchBox->blockSignals(false);
            }
        }
    });

    connect(m_completer, QOverload<const QString &>::of(&QCompleter::activated),
            [=](const QString &text) {
        Q_UNUSED(text);
        emit m_kiranSearchBox->returnPressed();
    });
}

void LanguageSelectDialog::addLanguageItem(const QString &languageName, const QString &locale)
{
    auto item = new QStandardItem(languageName);
    item->setData(locale);
    model->appendRow(item);
}