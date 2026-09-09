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
#include "printer-card.h"
#include "printer-resources.h"

#include <QHBoxLayout>
#include <QColor>
#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <QPalette>
#include <QPixmap>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#include <kiran-style/style-palette.h>

PrinterCard::PrinterCard(const PrinterInfo &info, QWidget *parent)
    : QFrame(parent),
      m_info(info)
{
    setObjectName(QStringLiteral("printerCard"));
    initUi();
    applyTheme();
    refreshUi();
    connect(Kiran::StylePalette::instance(), &Kiran::StylePalette::themeChanged, this, &PrinterCard::applyTheme);
}

void PrinterCard::setInfo(const PrinterInfo &info)
{
    if (m_info == info)
    {
        return;
    }
    m_info = info;
    refreshUi();
}

void PrinterCard::initUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(14, 12, 14, 12);
    root->setSpacing(10);

    auto *topRow = new QHBoxLayout();
    topRow->setSpacing(12);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setObjectName(QStringLiteral("printerIconLabel"));
    m_iconLabel->setFixedSize(40, 40);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    topRow->addWidget(m_iconLabel, 0, Qt::AlignTop);

    auto *infoCol = new QVBoxLayout();
    infoCol->setSpacing(4);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setObjectName(QStringLiteral("nameLabel"));
    m_nameLabel->setStyleSheet(QStringLiteral("font-size: 14px; font-weight: 600;"));
    infoCol->addWidget(m_nameLabel);

    auto *statusRow = new QHBoxLayout();
    statusRow->setSpacing(6);
    m_stateDot = new QLabel(this);
    m_stateDot->setObjectName(QStringLiteral("stateDot"));
    m_stateDot->setFixedSize(8, 8);
    statusRow->addWidget(m_stateDot, 0, Qt::AlignVCenter);
    m_statusLabel = new QLabel(this);
    m_statusLabel->setObjectName(QStringLiteral("statusLabel"));
    statusRow->addWidget(m_statusLabel);
    statusRow->addStretch(1);
    infoCol->addLayout(statusRow);

    m_modelLabel = new QLabel(this);
    m_modelLabel->setObjectName(QStringLiteral("modelLabel"));
    m_modelLabel->setStyleSheet(QStringLiteral("font-size: 12px;"));
    infoCol->addWidget(m_modelLabel);

    topRow->addLayout(infoCol, 1);

    m_defaultBox = new QWidget(this);
    auto *badgeLayout = new QHBoxLayout(m_defaultBox);
    badgeLayout->setContentsMargins(0, 0, 0, 0);
    badgeLayout->setSpacing(4);
    m_starLabel = new QLabel(m_defaultBox);
    m_starLabel->setObjectName(QStringLiteral("defaultStarLabel"));
    m_starLabel->setPixmap(QIcon(PrinterResources::starPath()).pixmap(16, 16));
    m_defaultBadge = new QLabel(tr("Default Printer"), m_defaultBox);
    m_defaultBadge->setObjectName(QStringLiteral("defaultBadge"));
    m_defaultBadge->setStyleSheet(QStringLiteral("font-size: 12px;"));
    badgeLayout->addWidget(m_starLabel);
    badgeLayout->addWidget(m_defaultBadge);
    topRow->addWidget(m_defaultBox, 0, Qt::AlignTop | Qt::AlignRight);

    root->addLayout(topRow);

    auto *actionRow = new QHBoxLayout();
    actionRow->setSpacing(8);

    m_setDefaultButton = new QPushButton(tr("Set as Default"), this);
    m_setDefaultButton->setObjectName(QStringLiteral("setDefaultButton"));
    connect(m_setDefaultButton, &QPushButton::clicked, this, [this]() {
        emit setDefaultClicked(m_info.name);
    });
    actionRow->addWidget(m_setDefaultButton);

    auto *testButton = new QPushButton(tr("Print Test Page"), this);
    testButton->setObjectName(QStringLiteral("printTestPageButton"));
    connect(testButton, &QPushButton::clicked, this, [this]() {
        emit printTestPageClicked(m_info.name);
    });
    actionRow->addWidget(testButton);

    auto *propsButton = new QPushButton(tr("Properties"), this);
    propsButton->setObjectName(QStringLiteral("propertiesButton"));
    connect(propsButton, &QPushButton::clicked, this, [this]() {
        emit propertiesClicked(m_info.name);
    });
    actionRow->addWidget(propsButton);

    auto *cardMenuButton = new QToolButton(this);
    cardMenuButton->setObjectName(QStringLiteral("cardMenuButton"));
    cardMenuButton->setText(QStringLiteral("…"));
    cardMenuButton->setPopupMode(QToolButton::InstantPopup);
    auto *cardMenu = new QMenu(cardMenuButton);
    QAction *viewQueue = cardMenu->addAction(tr("View Print Queue"));
    connect(viewQueue, &QAction::triggered, this, [this]() {
        emit viewQueueClicked(m_info.name);
    });
    QAction *managePrinter = cardMenu->addAction(tr("Manage Printer"));
    connect(managePrinter, &QAction::triggered, this, &PrinterCard::managePrinterClicked);
    cardMenuButton->setMenu(cardMenu);
    actionRow->addWidget(cardMenuButton);
    actionRow->addStretch(1);

    root->addLayout(actionRow);
}

void PrinterCard::applyTheme()
{
    auto *stylePalette = Kiran::StylePalette::instance();
    const QColor bg = stylePalette->color(Kiran::StylePalette::Normal,
                                          Kiran::StylePalette::Window,
                                          Kiran::StylePalette::Background);
    const QColor border = stylePalette->color(Kiran::StylePalette::Normal,
                                              Kiran::StylePalette::Widget,
                                              Kiran::StylePalette::Border);
    setStyleSheet(QStringLiteral("PrinterCard {"
                                 "  background-color: %1;"
                                 "  border: 1px solid %2;"
                                 "  border-radius: 10px;"
                                 "}")
                      .arg(bg.name(), border.name()));

    QPalette badgePal;
    badgePal.setColor(QPalette::WindowText, QColor(QStringLiteral("#f0c040")));
    m_defaultBadge->setPalette(badgePal);
    update();
}

void PrinterCard::refreshUi()
{
    m_iconLabel->setPixmap(PrinterResources::printerItemPixmap(m_info.isOnline));
    m_nameLabel->setText(m_info.name);
    m_stateDot->setStyleSheet(m_info.isOnline ? QStringLiteral("background:#2ecc71;border-radius:4px;")
                                              : QStringLiteral("background:#999999;border-radius:4px;"));
    m_statusLabel->setText(m_info.isOnline ? tr("Ready") : tr("Offline"));
    const QString modelText = m_info.model.isEmpty() ? QStringLiteral("—") : m_info.model;
    m_modelLabel->setText(tr("Model: %1").arg(modelText));
    m_defaultBox->setVisible(m_info.isDefault);
    m_setDefaultButton->setEnabled(!m_info.isDefault);
}
