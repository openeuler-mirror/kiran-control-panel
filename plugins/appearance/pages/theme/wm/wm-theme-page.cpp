/**
 * Copyright (c) 2020 ~ 2025 KylinSec Co., Ltd.
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
#include "wm-theme-page.h"
#include <kiran-session-daemon/appearance-i.h>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QTimer>
#include "appearance-global-info.h"
#include "config.h"
#include "exclusion-group.h"
#include "theme-preview-widget.h"
#include "ui_wm-theme-page.h"

#define WINDOW_THUMBNAIL_WIDTH 420
#define WINDOW_THUMBNAIL_HEIGHT 60

WMThemePage::WMThemePage(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::WMThemePage)
{
    ui->setupUi(this);
    init();
}

WMThemePage::~WMThemePage()
{
    delete ui;
}

void WMThemePage::updateCurrentTheme(QString newTheme)
{
    QSignalBlocker blocker(m_exclusionGroup);
    m_exclusionGroup->setCurrent(newTheme);
}

ThemePreviewWidget *WMThemePage::createPreviewWidget(const QString &themeName, const QList<QPixmap> pixmaps, bool selected)
{
    auto previewWidget = new ThemePreviewWidget(this);
    previewWidget->setSpacingAndMargin(0, QMargins(24, 0, 24, 0));
    previewWidget->setSelectedIndicatorEnable(true);
    previewWidget->setSelected(selected);
    previewWidget->setThemeInfo(themeName, themeName);
    previewWidget->setPreviewPixmapSize(QSize(WINDOW_THUMBNAIL_WIDTH, WINDOW_THUMBNAIL_HEIGHT));
    previewWidget->setPreviewPixmaps(pixmaps);
    connect(previewWidget, &ThemePreviewWidget::pressed, this, [this]()
            { emit requestReturn(); });
    m_exclusionGroup->addExclusionItem(previewWidget);
    return previewWidget;
}

void WMThemePage::processThumbnailCacheChanged()
{
    QDir dir(m_thumbaniPath);
    auto fileInfos = dir.entryInfoList(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    QMap<QString, ThemePreviewWidget *> previewWidgetMap;
    const auto contentLayout = ui->layout_scrollAreaWidgetContents;
    for (int i = 0; i < contentLayout->count(); i++)
    {
        auto item = contentLayout->itemAt(i);
        if (!item->widget())
            continue;

        auto previewWidget = qobject_cast<ThemePreviewWidget *>(item->widget());
        if (!previewWidget)
            continue;

        previewWidgetMap[previewWidget->getID()] = previewWidget;
    }

    for (auto fileInfo : fileInfos)
    {
        if (fileInfo.suffix() != "png" ||
            !previewWidgetMap.contains(fileInfo.baseName()))
        {
            continue;
        }

        auto themeName = fileInfo.baseName();
        auto preview = loadPreview(themeName);
        auto previewWidget = previewWidgetMap[themeName];
        previewWidget->setPreviewPixmaps({preview});
    }
}

void WMThemePage::createThumbnailCache()
{
    const QString cacheLocation = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
    m_thumbaniPath = QString("%1/wm-thumbnails").arg(cacheLocation);

    QDir thumbaniDir(m_thumbaniPath);
    if (!thumbaniDir.exists())
    {
        thumbaniDir.mkpath(".");
    }

    // 避免短时间多次写入多次触发
    m_loadThumbnailTimer = new QTimer(this);
    m_loadThumbnailTimer->setSingleShot(true);
    m_loadThumbnailTimer->setInterval(200);
    connect(m_loadThumbnailTimer, &QTimer::timeout, this, &WMThemePage::processThumbnailCacheChanged);

    // 缩略图缓存目录发生改变触发重新加载
    QFileSystemWatcher *watcher = new QFileSystemWatcher({m_thumbaniPath}, this);
    connect(watcher, &QFileSystemWatcher::directoryChanged, this, [this]()
            { m_loadThumbnailTimer->start(); });
}

void WMThemePage::init()
{
    createThumbnailCache();

    m_exclusionGroup = new ExclusionGroup(this);
    connect(m_exclusionGroup, &ExclusionGroup::currentItemChanged, this, &WMThemePage::processCurrentItemChanged);

    QString currentTheme;
    AppearanceGlobalInfo::instance()->getTheme(APPEARANCE_THEME_TYPE_METACITY, currentTheme);

    QStringList noThumbnailerThemes;
    auto themeInfos = AppearanceGlobalInfo::instance()->getAllThemes(APPEARANCE_THEME_TYPE_METACITY);
    for (auto themeInfo : themeInfos)
    {
        auto preview = loadPreview(themeInfo.name);
        if (!preview)
        {
            noThumbnailerThemes.append(themeInfo.name);
        }
        auto widget = createPreviewWidget(themeInfo.name, {preview}, themeInfo.name == currentTheme);
        ui->layout_scrollAreaWidgetContents->addWidget(widget);
    }

    // 缺失部分主题缩略图，调用生成命令
    if (!noThumbnailerThemes.isEmpty())
    {
        generateWMThemePreview(noThumbnailerThemes);
    }
}

void WMThemePage::processCurrentItemChanged()
{
    auto current = m_exclusionGroup->getCurrent();
    auto id = current->getID();

    KLOG_INFO(qLcAppearance) << "wm theme ui current changed:" << id;
    if (!AppearanceGlobalInfo::instance()->setTheme(APPEARANCE_THEME_TYPE_METACITY, id))
    {
        KLOG_WARNING(qLcAppearance) << "set currnet wm theme" << id << "failed!";
    }
    else
    {
        KLOG_INFO(qLcAppearance) << "wm theme updated:" << id;
    }
}

QPixmap WMThemePage::loadPreview(const QString &themeName)
{
    QString previewPath = QString("%1/%2.png").arg(m_thumbaniPath, themeName);
    if (QFileInfo::exists(previewPath))
    {
        QPixmap pixmap(previewPath);
        if (pixmap.width() == WINDOW_THUMBNAIL_WIDTH &&
            pixmap.height() == WINDOW_THUMBNAIL_HEIGHT)
        {
            return pixmap;
        }
        KLOG_DEBUG(qLcAppearance) << "window theme" << themeName << "thumbnail size invalid:" << pixmap.size();
    }
    else
    {
        KLOG_DEBUG(qLcAppearance) << previewPath << "thumbnail not exist";
    }
    return QPixmap();
}

// 启动初次加载所有窗口主题时调用，生成缺失的窗口主题缩略图
void WMThemePage::generateWMThemePreview(const QStringList &themeNames)
{
    QStringList args = {
        "--thumbnail-directory", m_thumbaniPath,
        "--thumbnail-width", QString::number(WINDOW_THUMBNAIL_WIDTH),
        "--thumbnail-height", QString::number(WINDOW_THUMBNAIL_HEIGHT)};

    for (auto name : themeNames)
    {
        args << "--theme-name" << name;
    }

    static const char *thumbnailer = "/usr/libexec/wm-theme-thumbnailer";
    KLOG_INFO(qLcAppearance) << thumbnailer << args;

    QProcess::startDetached(thumbnailer, args);
}
