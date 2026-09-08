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
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */
#include "font-page.h"
#include "font-context-menu.h"
#include "font-manager.h"
#include "font-table.h"
#include "logging-category.h"
#include "ui_font-page.h"

#include <kiran-message-box.h>
#include <style-property.h>
#include <QApplication>
#include <QFileDialog>
#include <QFont>
#include <QMenu>
#include <QPalette>
#include <QProcess>
#include <QStandardPaths>

using namespace Kiran;

FontPage::FontPage(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::FontPage)
{
    ui->setupUi(this);
    initUI();
    initConnections();
    reloadFonts();
}

FontPage::~FontPage()
{
    delete ui;
}

QSize FontPage::sizeHint() const
{
    return QSize(670, 730);
}

QWidget *FontPage::createPage()
{
    return new FontPage();
}

void FontPage::initUI()
{
    QFont titleFont = ui->label_title->font();
    titleFont.setPointSize(titleFont.pointSize() + 3);
    titleFont.setBold(true);
    ui->label_title->setFont(titleFont);

    QPalette descPalette = ui->label_desc->palette();
    descPalette.setColor(QPalette::WindowText, descPalette.color(QPalette::Disabled, QPalette::WindowText));
    ui->label_desc->setPalette(descPalette);

    StylePropertyHelper::setButtonType(ui->btn_add, BUTTON_Default);
    StylePropertyHelper::setButtonType(ui->btn_delete, BUTTON_Normal);
    ui->btn_add->setIcon(QIcon(":/kcp-font/images/add.svg"));
    ui->btn_delete->setIcon(QIcon(":/kcp-font/images/trash.svg"));
    ui->btn_delete->setEnabled(false);

    ui->edit_search->setPlaceholderText(tr("Search Font"));
}

void FontPage::initConnections()
{
    connect(ui->btn_add, &QPushButton::clicked, this, [this]()
            {
                QMenu menu(this);
                menu.addAction(tr("System Font"), this, &FontPage::onAddSystemFont);
                menu.addAction(tr("User Font"), this, &FontPage::onAddUserFont);
                menu.exec(ui->btn_add->mapToGlobal(QPoint(0, ui->btn_add->height())));
            });
    connect(ui->btn_delete, &QPushButton::clicked, this, &FontPage::onDeleteFonts);
    connect(ui->edit_search, &QLineEdit::textChanged, ui->table_fonts, &FontTable::searchTextChanged);
    connect(ui->table_fonts, &FontTable::fontSelectedChanged, this, [this](int)
            { updateDeleteButtonState(); });
    connect(ui->table_fonts, &FontTable::customContextMenuRequested, this, &FontPage::onTableContextMenu);
}

void FontPage::reloadFonts()
{
    ui->table_fonts->setFonts(FontManager::listFonts());
    updateDeleteButtonState();
}

void FontPage::updateDeleteButtonState()
{
    ui->btn_delete->setEnabled(!ui->table_fonts->selectedFonts().isEmpty());
}

QList<FontInfo> FontPage::targetFonts(const QPoint &contextPos) const
{
    const QList<FontInfo> checked = ui->table_fonts->selectedFonts();
    if (contextPos.isNull())
        return checked;

    const FontInfo clicked = ui->table_fonts->fontAt(contextPos);
    if (clicked.family.isEmpty())
        return checked;

    Q_FOREACH (const FontInfo &font, checked)
    {
        if (font.identity() == clicked.identity())
            return checked;
    }

    QList<FontInfo> fonts;
    fonts.append(clicked);
    return fonts;
}

QStringList FontPage::selectFontFiles()
{
    return QFileDialog::getOpenFileNames(this,
                                         tr("Select Font Files"),
                                         QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
                                         tr("Font Files (*.ttf *.otf *.ttc *.otc *.woff *.woff2 *.pcf *.bdf *.pfa *.pfb);;All Files (*)"));
}

void FontPage::addFonts(bool system)
{
    const QStringList files = selectFontFiles();
    if (files.isEmpty())
        return;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString error;
    const bool ok = FontManager::addFonts(files, system, &error);
    QApplication::restoreOverrideCursor();

    if (!ok)
    {
        showError(error);
        return;
    }
    reloadFonts();
}

void FontPage::deleteFonts(const QList<FontInfo> &fonts)
{
    if (fonts.isEmpty())
        return;

    const QString message = fonts.size() == 1
                                ? tr("Are you sure you want to delete font \"%1 %2\"?").arg(fonts.first().family).arg(fonts.first().style)
                                : tr("Are you sure you want to delete the selected %1 fonts?").arg(fonts.size());
    if (KiranMessageBox::message(this, tr("Delete Font"), message,
                                 KiranMessageBox::Yes | KiranMessageBox::No) != KiranMessageBox::Yes)
    {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString error;
    const bool ok = FontManager::removeFonts(fonts, &error);
    QApplication::restoreOverrideCursor();

    if (!ok)
    {
        showError(error);
        return;
    }
    reloadFonts();
}

void FontPage::setFontsEnabled(const QList<FontInfo> &fonts, bool enabled)
{
    if (fonts.isEmpty())
        return;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString error;
    const bool ok = FontManager::setFontsEnabled(fonts, enabled, &error);
    QApplication::restoreOverrideCursor();

    if (!ok)
    {
        showError(error);
        return;
    }
    reloadFonts();
}

void FontPage::openFontViewer(const FontInfo &info)
{
    const QString file = info.primaryFile();
    if (file.isEmpty())
        return;

    const QString viewer = QStandardPaths::findExecutable(QStringLiteral("mate-font-viewer"));
    if (viewer.isEmpty() || !QProcess::startDetached(viewer, QStringList() << file))
        showError(tr("Failed to open the font viewer."));
}

void FontPage::showError(const QString &message)
{
    KLOG_WARNING(qLcFont) << message;
    KiranMessageBox::message(this, tr("Error"), message, KiranMessageBox::Ok);
}

void FontPage::onAddSystemFont()
{
    addFonts(true);
}

void FontPage::onAddUserFont()
{
    addFonts(false);
}

void FontPage::onDeleteFonts()
{
    deleteFonts(targetFonts());
}

void FontPage::onTableContextMenu(const QPoint &pos)
{
    const FontInfo clicked = ui->table_fonts->fontAt(pos);
    if (clicked.family.isEmpty())
        return;

    const QList<FontInfo> fonts = targetFonts(pos);
    if (fonts.isEmpty())
        return;

    bool hasEnabled = false;
    bool hasDisabled = false;
    Q_FOREACH (const FontInfo &font, fonts)
    {
        if (font.enabled)
            hasEnabled = true;
        else
            hasDisabled = true;
    }

    FontContextMenu menu(this);
    QAction *disableAction = menu.addAction(QIcon(":/kcp-font/images/disabled.svg"), tr("Disable"));
    disableAction->setEnabled(hasEnabled);
    QAction *enableAction = menu.addAction(QIcon(":/kcp-font/images/enabled.svg"), tr("Enable"));
    enableAction->setEnabled(hasDisabled);
    QAction *deleteAction = menu.addAction(QIcon(":/kcp-font/images/trash.svg"), tr("Delete"));
    QAction *openAction = nullptr;
    if (!QStandardPaths::findExecutable(QStringLiteral("mate-font-viewer")).isEmpty())
    {
        menu.addSeparator();
        openAction = menu.addAction(QIcon::fromTheme("kcp-appearance-font"),
                                    tr("Open with Font Viewer"));
    }

    QAction *selected = menu.exec(ui->table_fonts->viewport()->mapToGlobal(pos));
    if (!selected)
        return;

    if (selected == disableAction)
        setFontsEnabled(fonts, false);
    else if (selected == enableAction)
        setFontsEnabled(fonts, true);
    else if (selected == deleteAction)
        deleteFonts(fonts);
    else if (openAction && selected == openAction)
        openFontViewer(clicked);
}
