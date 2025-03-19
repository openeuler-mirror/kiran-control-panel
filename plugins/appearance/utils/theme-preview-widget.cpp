/**
 * Copyright (c) 2020 ~ 2023 KylinSec Co., Ltd.
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
#include "theme-preview-widget.h"
#include <kiran-frame/kiran-frame.h>
#include <QBoxLayout>
#include <QLabel>
#include <QEvent>
#include <QLabel>
#include <QVariant>

ThemePreviewWidget::ThemePreviewWidget(QWidget* parent)
    : ExclusionWidget(parent)
{
    initUI();
}

ThemePreviewWidget::~ThemePreviewWidget()
{
}

void ThemePreviewWidget::setPreviewFixedHeight(int height)
{
    m_frame->setFixedHeight(height);
}

void ThemePreviewWidget::setPreviewFixedSize(QSize size)
{
    m_frame->setFixedSize(size);
}

void ThemePreviewWidget::setSpacingAndMargin(int spacing, QMargins margins)
{
    m_frameLayout->setSpacing(spacing);
    m_frameLayout->setContentsMargins(margins);
}

void ThemePreviewWidget::setSelectedIndicatorEnable(bool enable)
{
    m_selectedIndicatorEnable = enable;

    if (m_selectedIndicator->isVisible())
    {
        m_selectedIndicator->setVisible(false);
    }

    if (!m_selectedIndicatorEnable)
    {
        m_frame->setDrawBroder(false);
    }
}

void ThemePreviewWidget::setSelectedBorderWidth(int width)
{
    m_frame->setBorderWidth(width);
}

void ThemePreviewWidget::setThemeInfo(const QString& name,
                                      const QString& id)
{
    m_themeName = name;
    m_themeID = id;

    m_labelThemeName->setText(m_themeName);
}

void ThemePreviewWidget::setPreviewPixmapSize(QSize size)
{
    m_previewSize = size;
}

void ThemePreviewWidget::setPreviewPixmaps(const QList<QPixmap>& pixmaps)
{
    static const char* preview_pixmap_property = "_theme_preview_flag_";

    // 清理
    for (int i = 0; i < m_frameLayout->count();)
    {
        auto item = m_frameLayout->itemAt(i);
        auto widget = item->widget();

        if (widget && !widget->property(preview_pixmap_property).isNull())
        {
            m_frameLayout->removeWidget(widget);
            delete widget;
            continue;
        }

        i++;
    }

    for (auto pixmap : pixmaps)
    {
        auto labelPixmap = new QLabel(this);
        labelPixmap->setProperty(preview_pixmap_property, QVariant(1));
        labelPixmap->setFixedSize(m_previewSize);
        labelPixmap->setPixmap(pixmap.scaled(m_previewSize,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

        // 插入在选择指示器之前
        m_frameLayout->insertWidget(m_frameLayout->count()-2,labelPixmap,0,Qt::AlignCenter);
    }
}

QString ThemePreviewWidget::getID() const
{
    return m_themeID;
}

void ThemePreviewWidget::setSelected(bool selected)
{
    if( selected )
    {
        if( m_selectedIndicatorEnable )
        {
            m_selectedIndicator->setVisible(true);
            m_frame->setDrawBroder(true);
        }
    }
    else
    {
        m_selectedIndicator->setVisible(false);
        m_frame->setDrawBroder(false);
    }

    ExclusionWidget::setSelected(selected);
}

void ThemePreviewWidget::initUI()
{
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(6);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 主题展示Frame
    m_frame = new KiranFrame(this);
    m_frame->setCursor(Qt::PointingHandCursor);
    m_frame->setObjectName("ThemePreviewFrame");
    m_frame->setDrawBroder(false);
    m_frame->setFixedBorderState(Kiran::StylePalette::Checked);
    m_frame->installEventFilter(this);

    m_frameLayout = new QHBoxLayout(m_frame);
    m_frameLayout->setContentsMargins(0, 0, 0, 0);
    m_frameLayout->setSpacing(0);

    m_selectedIndicator = new QLabel(m_frame);
    m_selectedIndicator->setFixedSize(QSize(16, 16));
    m_selectedIndicator->setVisible(false);
    m_selectedIndicator->setPixmap(QPixmap(":/kcp-appearance/images/indicator-selected.png"));

    auto spacerItem = new QSpacerItem(0,10,QSizePolicy::Expanding,QSizePolicy::Preferred);
    m_frameLayout->addSpacerItem(spacerItem);
    m_frameLayout->addWidget(m_selectedIndicator);

    mainLayout->addWidget(m_frame);

    // 主题名字
    m_labelThemeName = new QLabel(this);
    m_labelThemeName->setObjectName("ThemePreviewName");
    m_labelThemeName->setStyleSheet("#ThemePreviewName{color: #919191;"
                                    "font-family: Noto Sans CJK SC regular;"
                                    "font-size: 14px}");
    m_labelThemeName->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_labelThemeName, Qt::AlignHCenter);
}

bool ThemePreviewWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_frame && event->type() == QEvent::MouseButtonPress)
    {
        setSelected(true);
        emit pressed();
    }
    return false;
}
