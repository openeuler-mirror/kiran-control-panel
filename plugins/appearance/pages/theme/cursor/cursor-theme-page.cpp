/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
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
#include "cursor-theme-page.h"
#include <kiran-log/qt5-log-i.h>
#include <kiran-message-box.h>
#include <kiran-session-daemon/appearance-i.h>
#include <QDir>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLabel>
#include <QVBoxLayout>
#include "appearance-global-info.h"
#include "cursor-image-loader.h"
#include "exclusion-group.h"
#include "theme-preview-widget.h"

QStringList CursorThemePage::m_previewCursors = {"left_ptr", "right_ptr", "top_left_corner", "top_right_corner", "size_hor", "pointer"};

CursorThemePage::CursorThemePage(QWidget* parent) : QWidget(parent)
{
    init();
}

CursorThemePage::~CursorThemePage()
{
}

void CursorThemePage::init()
{
    initUI();
    loadCurosrThemes();
}

void CursorThemePage::initUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(10);

    QLabel* label_text = new QLabel(this);
    label_text->setText(tr("Cursor Themes Settings"));
    layout->addWidget(label_text);

    m_exclusionGroup = new ExclusionGroup(this);
    connect(m_exclusionGroup,&ExclusionGroup::currentItemChanged,this,&CursorThemePage::onCurrentItemChanged);

    QWidget* cursorsContainer = new QWidget(this);
    m_cursorVlayout = new QVBoxLayout(cursorsContainer);
    m_cursorVlayout->setSpacing(16);
    m_cursorVlayout->setMargin(0);
    layout->addWidget(cursorsContainer);

    layout->addStretch();
}

void CursorThemePage::loadCurosrThemes()
{
    auto themeInfos = AppearanceGlobalInfo::instance()->getAllThemes(APPEARANCE_THEME_TYPE_CURSOR);

    QString currentCursorTheme;
    AppearanceGlobalInfo::instance()->getTheme(APPEARANCE_THEME_TYPE_CURSOR, currentCursorTheme);

    for (auto theme : themeInfos)
    {
        const QString& cursorThemePath = theme.path;
        QDir cursorThemeDir(cursorThemePath);
        const QString& themeBaseName = cursorThemeDir.dirName();

        QList<QPixmap> themeCursors;
        for (auto cursorName : m_previewCursors)
        {
            auto cursorImg = CursorImageLoader::getCursorImage(themeBaseName, cursorName, 22);
            auto curosrPixmap = QPixmap::fromImage(cursorImg);
            themeCursors << curosrPixmap;
        }

        auto previewWidget = createPreviewWidget(theme.name,themeCursors,theme.name==currentCursorTheme);
        m_exclusionGroup->addExclusionItem(previewWidget);
        m_cursorVlayout->addWidget(previewWidget);
    }
}

ThemePreviewWidget* CursorThemePage::createPreviewWidget(const QString& themeName,
                                                         const QList<QPixmap> pixmaps,
                                                         bool selected)
{
    auto previewWidget = new ThemePreviewWidget(this);
    previewWidget->setPreviewFixedHeight(40);
    previewWidget->setSpacingAndMargin(24, QMargins(24, 0, 24, 0));
    previewWidget->setSelectedIndicatorEnable(true);
    previewWidget->setThemeInfo(themeName, themeName);
    previewWidget->setPreviewPixmapSize(QSize(22, 22));
    previewWidget->setPreviewPixmaps(pixmaps);
    previewWidget->setSelected(selected);

    connect(previewWidget, &ThemePreviewWidget::pressed, this, [this]()
            { emit requestReturn(); });
    return previewWidget;
}

void CursorThemePage::onCurrentItemChanged()
{
    auto current = m_exclusionGroup->getCurrent();
    auto id = current->getID();

    KLOG_INFO(qLcAppearance) << "cursor theme ui current changed:" << id;
    if( !AppearanceGlobalInfo::instance()->setTheme(APPEARANCE_THEME_TYPE_CURSOR,id) )
    {
        KLOG_WARNING(qLcAppearance) << "set current cursor theme" << id << "failed!";
    }
    else
    {
        KLOG_INFO(qLcAppearance) << "cursor theme updated:" << id;
    }

}

void CursorThemePage::updateCurrentTheme(QString newCursorTheme)
{
    QSignalBlocker blocker(m_exclusionGroup);
    m_exclusionGroup->setCurrent(newCursorTheme);
}