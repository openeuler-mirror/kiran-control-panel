/**
 * Copyright (c) 2026 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     gaobo <gaobo@kylinsec.com.cn>
 */
#include "printer-page.h"

#include <QColor>
#include <QLineEdit>
#include <QPalette>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSize>
#include <QTimer>
#include <QVBoxLayout>

#include "printer-card.h"
#include "printer-manager.h"
#include "printer-resources.h"
#include "ui_printer-page.h"

#include <kiran-style/style-palette.h>

namespace
{
bool sameCardNames(const QList<PrinterCard *> &cards, const QList<PrinterInfo> &printers)
{
    if (cards.size() != printers.size())
    {
        return false;
    }
    for (int i = 0; i < cards.size(); ++i)
    {
        if (cards.at(i)->info().name != printers.at(i).name)
        {
            return false;
        }
    }
    return true;
}
}  // namespace

PrinterPage::PrinterPage(PrinterManager *manager, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::PrinterPage),
      m_manager(manager)
{
    Q_ASSERT(m_manager);
    m_manager->setParent(this);
    ui->setupUi(this);

    initUiStyle();
    initConnect();
    m_manager->start();
}

PrinterPage::~PrinterPage()
{
    m_manager->stop();
    m_cards.clear();
    delete ui;
}

QSize PrinterPage::sizeHint() const
{
    return {640, 640};
}

void PrinterPage::initUiStyle()
{
    QPixmap warningPix(PrinterResources::warningPath());
    ui->errorIconLabel->setPixmap(warningPix.scaled(72, 72, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    ui->refreshButton->setFlat(true);
    ui->refreshButton->setFocusPolicy(Qt::NoFocus);
    ui->refreshButton->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "  color: #23beff;"
        "  background-color: transparent;"
        "  background-image: url(:/kcp-printer/images/refresh.svg);"
        "  background-repeat: no-repeat;"
        "  background-position: left center;"
        "  border: none;"
        "  padding-left: 20px;"
        "}"
        "QPushButton:hover {"
        "  color: #65d2ff;"
        "  background-color: transparent;"
        "  background-image: url(:/kcp-printer/images/refresh-hover.svg);"
        "}"
        "QPushButton:pressed {"
        "  color: #1c98cc;"
        "  background-color: transparent;"
        "  background-image: url(:/kcp-printer/images/refresh-pressed.svg);"
        "}"));

    ui->printerScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->errorTitleLabel->setStyleSheet(QStringLiteral("font-size: 16px; font-weight: 600;"));

    applyTheme();
    connect(Kiran::StylePalette::instance(), &Kiran::StylePalette::themeChanged, this, &PrinterPage::applyTheme);
}

void PrinterPage::applyTheme()
{
    auto *stylePalette = Kiran::StylePalette::instance();
    const QColor border = stylePalette->color(Kiran::StylePalette::Normal,
                                              Kiran::StylePalette::Widget,
                                              Kiran::StylePalette::Border);

    ui->errorDescLabel->setMaximumWidth(420);

    ui->footerSeparator->setAutoFillBackground(true);
    QPalette sepPal = ui->footerSeparator->palette();
    sepPal.setColor(QPalette::Window, border);
    ui->footerSeparator->setPalette(sepPal);
}

void PrinterPage::initConnect()
{
    connect(ui->addPrinterButton, &QPushButton::clicked, this, &PrinterPage::handleAddPrinterClicked);
    connect(ui->startServiceButton, &QPushButton::clicked, this, &PrinterPage::handleStartServiceClicked);
    connect(ui->retryButton, &QPushButton::clicked, this, &PrinterPage::handleRefresh);
    connect(ui->refreshButton, &QPushButton::clicked, this, &PrinterPage::handleRefresh);
    connect(ui->searchEdit, &QLineEdit::textChanged, this, &PrinterPage::handleSearchTextChanged);

    connect(m_manager, &PrinterManager::serviceStatusChanged, this, &PrinterPage::handleServiceStatusChanged);
    connect(m_manager, &PrinterManager::printersChanged, this, &PrinterPage::handlePrintersChanged);
}

void PrinterPage::setServiceControlsEnabled(bool enabled)
{
    ui->searchEdit->setEnabled(enabled);
    ui->addPrinterButton->setEnabled(enabled);
}

void PrinterPage::clearPrinterCards()
{
    m_cards.clear();
    while (ui->printerCardsLayout->count() > 0)
    {
        QLayoutItem *item = ui->printerCardsLayout->takeAt(0);
        delete item->widget();
        delete item;
    }
}

bool PrinterPage::printerMatchesKeyword(const PrinterInfo &info, const QString &keyword) const
{
    if (keyword.isEmpty())
    {
        return true;
    }
    return info.name.contains(keyword, Qt::CaseInsensitive) ||
           info.model.contains(keyword, Qt::CaseInsensitive);
}

void PrinterPage::bindCard(PrinterCard *card)
{
    connect(card, &PrinterCard::setDefaultClicked, this, &PrinterPage::handleSetDefaultClicked);
    connect(card, &PrinterCard::printTestPageClicked, this, &PrinterPage::handlePrintTestPageClicked);
    connect(card, &PrinterCard::propertiesClicked, this, &PrinterPage::handlePropertiesClicked);
    connect(card, &PrinterCard::viewQueueClicked, this, &PrinterPage::handleViewQueueClicked);
    connect(card, &PrinterCard::managePrinterClicked, m_manager, &PrinterManager::openManagePrinter);
}

void PrinterPage::rebuildPrinterCards(const QList<PrinterInfo> &printers)
{
    QScrollBar *bar = ui->printerScrollArea->verticalScrollBar();
    const int savedY = bar->value();

    clearPrinterCards();
    for (const PrinterInfo &info : printers)
    {
        auto *card = new PrinterCard(info, ui->printerListContainer);
        bindCard(card);
        m_cards.append(card);
        ui->printerCardsLayout->addWidget(card);
    }
    ui->printerCardsLayout->addStretch(1);

    QTimer::singleShot(0, this, [bar, savedY]() {
        bar->setValue(savedY);
    });
}

void PrinterPage::syncCards(const QList<PrinterInfo> &printers)
{
    if (sameCardNames(m_cards, printers))
    {
        for (int i = 0; i < m_cards.size(); ++i)
        {
            m_cards.at(i)->setInfo(printers.at(i));
        }
        return;
    }

    rebuildPrinterCards(printers);
}

void PrinterPage::applySearchFilter()
{
    const QString keyword = ui->searchEdit->text().trimmed();
    int visibleCount = 0;
    for (PrinterCard *card : m_cards)
    {
        const bool visible = printerMatchesKeyword(card->info(), keyword);
        card->setVisible(visible);
        if (visible)
        {
            ++visibleCount;
        }
    }
    ui->countLabel->setText(tr("%1 printer(s)").arg(visibleCount));
}

void PrinterPage::showNormalState(const QList<PrinterInfo> &printers)
{
    m_serviceAvailable = true;
    m_allPrinters = printers;

    ui->stackedWidget->setCurrentWidget(ui->normalPage);
    setServiceControlsEnabled(true);

    ui->statusBarLabel->clear();
    ui->countLabel->setVisible(true);
    syncCards(m_allPrinters);
    applySearchFilter();
}

void PrinterPage::showErrorState()
{
    m_serviceAvailable = false;
    m_allPrinters.clear();

    ui->stackedWidget->setCurrentWidget(ui->errorPage);
    setServiceControlsEnabled(false);
    clearPrinterCards();
    ui->printerCardsLayout->addStretch(1);

    ui->countLabel->setVisible(false);
    ui->statusBarLabel->setText(tr("Print service unavailable"));
}

void PrinterPage::handleServiceStatusChanged(bool available)
{
    if (available)
    {
        m_manager->poll();
    }
    else
    {
        showErrorState();
    }
}

void PrinterPage::handlePrintersChanged(const QList<PrinterInfo> &printers)
{
    showNormalState(printers);
}

void PrinterPage::handleSearchTextChanged(const QString &text)
{
    Q_UNUSED(text);
    if (!m_serviceAvailable)
    {
        return;
    }
    applySearchFilter();
}

void PrinterPage::handleRefresh()
{
    m_manager->poll();
}

void PrinterPage::handleAddPrinterClicked()
{
    if (!m_manager->openAddDialog(quint32(winId())))
    {
        m_manager->poll();
    }
}

void PrinterPage::handleStartServiceClicked()
{
    // 异步发起：启动成功与否由服务状态变化信号驱动界面切换
    m_manager->startService();
}

void PrinterPage::handleSetDefaultClicked(const QString &printerName)
{
    m_manager->setDefault(printerName);
}

void PrinterPage::handlePrintTestPageClicked(const QString &printerName)
{
    m_manager->printTestPage(printerName, tr("Test Page"));
}

void PrinterPage::handlePropertiesClicked(const QString &printerName)
{
    m_manager->openProperties(quint32(winId()), printerName);
}

void PrinterPage::handleViewQueueClicked(const QString &printerName)
{
    m_manager->openPrintQueue(printerName);
}
