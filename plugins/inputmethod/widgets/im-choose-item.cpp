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

#include "im-choose-item.h"

#include <style-palette.h>

#include <QCheckBox>
#include <QEvent>
#include <QFontMetrics>
#include <QIcon>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>

using namespace Kiran;

namespace
{
// 条目几何常量
const int kItemHeight = 44;
const int kMarginLeft = 8;
const int kMarginRight = 12;
const int kCheckBoxWidth = 20;
const int kCheckBoxTextGap = 8;
const int kIconSize = 24;
const int kIconTextGap = 10;
const int kLangLabelHeight = 18;
const int kLangLabelPadding = 6;
const int kLangLabelRadius = 3;
const int kSpacing = 8;
}  // namespace

ImChooseItem::ImChooseItem(QWidget* parent)
    : QWidget(parent)
{
    initUI();
}

ImChooseItem::~ImChooseItem()
{
}

void ImChooseItem::initUI()
{
    setObjectName("ImChooseItem");
    setAttribute(Qt::WA_Hover);
    setFixedHeight(kItemHeight);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // 复选框交由 QCheckBox 绘制，自动跟随 Kiran 样式
    m_checkBox = new QCheckBox(this);
    m_checkBox->setObjectName("checkBox_select");
    m_checkBox->setFocusPolicy(Qt::TabFocus);

    connect(m_checkBox, &QCheckBox::toggled, this,
            [this](bool checked)
            {
                update();
                emit toggled(checked);
            });
}

void ImChooseItem::setEntry(const ImEntry& entry)
{
    m_entry = entry;
    update();
}

void ImChooseItem::setChecked(bool checked)
{
    m_checkBox->setChecked(checked);
}

bool ImChooseItem::isChecked() const
{
    return m_checkBox->isChecked();
}

void ImChooseItem::setAlreadyEnabled(bool alreadyEnabled)
{
    m_alreadyEnabled = alreadyEnabled;

    // 已启用项：勾上并禁用交互，视觉上整行置灰
    if (alreadyEnabled)
    {
        m_checkBox->setChecked(true);
    }
    m_checkBox->setEnabled(!alreadyEnabled);
    setToolTip(alreadyEnabled ? tr("This input method has already been added") : QString());
    update();
}

bool ImChooseItem::matches(const QString& keyword) const
{
    if (keyword.isEmpty())
    {
        return true;
    }

    // 简单包含匹配，大小写不敏感；743 个候选下足够
    return m_entry.uniqueName.contains(keyword, Qt::CaseInsensitive) ||
           m_entry.name.contains(keyword, Qt::CaseInsensitive) ||
           m_entry.nativeName.contains(keyword, Qt::CaseInsensitive) ||
           m_entry.langCode.contains(keyword, Qt::CaseInsensitive);
}

QString ImChooseItem::primaryText() const
{
    if (!m_entry.name.isEmpty())
    {
        return m_entry.name;
    }
    if (!m_entry.nativeName.isEmpty())
    {
        return m_entry.nativeName;
    }
    return m_entry.uniqueName;
}

QString ImChooseItem::secondaryText() const
{
    if (!m_entry.uniqueName.isEmpty())
    {
        return m_entry.uniqueName;
    }
    return m_entry.langCode;
}

QRect ImChooseItem::langLabelRect() const
{
    if (m_entry.langCode.isEmpty())
    {
        return QRect();
    }

    QFont labelFont = font();
    labelFont.setPointSizeF(qMax(6.0, font().pointSizeF() - 1));
    QFontMetrics fm(labelFont);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    int labelWidth = fm.horizontalAdvance(m_entry.langCode) + kLangLabelPadding * 2;
#else
    int labelWidth = fm.width(m_entry.langCode) + kLangLabelPadding * 2;
#endif

    int x = width() - kMarginRight - labelWidth;
    int y = (height() - kLangLabelHeight) / 2;
    return QRect(x, y, labelWidth, kLangLabelHeight);
}

QSize ImChooseItem::sizeHint() const
{
    return QSize(360, kItemHeight);
}

QSize ImChooseItem::minimumSizeHint() const
{
    return QSize(180, kItemHeight);
}

void ImChooseItem::resizeEvent(QResizeEvent* event)
{
    if (event)
    {
        QWidget::resizeEvent(event);
    }

    int y = (height() - kCheckBoxWidth) / 2;
    m_checkBox->setGeometry(kMarginLeft, y, kCheckBoxWidth, kCheckBoxWidth);
}

void ImChooseItem::enterEvent(QEvent* event)
{
    m_hovered = true;
    update();
    QWidget::enterEvent(event);
}

void ImChooseItem::leaveEvent(QEvent* event)
{
    m_hovered = false;
    update();
    QWidget::leaveEvent(event);
}

void ImChooseItem::mouseReleaseEvent(QMouseEvent* event)
{
    // 整行可点击切换勾选，已启用项不响应
    if (event->button() == Qt::LeftButton && !m_alreadyEnabled)
    {
        if (!m_checkBox->geometry().contains(event->pos()))
        {
            m_checkBox->toggle();
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void ImChooseItem::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    StylePalette* palette = StylePalette::instance();
    // 已启用项整体走 DISABLED 色组，视觉上置灰
    StylePalette::ColorState group = m_alreadyEnabled ? StylePalette::Disabled
                                                      : StylePalette::Active;

    // 悬浮背景（已启用项不高亮）
    if (m_hovered && !m_alreadyEnabled)
    {
        QRectF backgroundRect = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
        QPainterPath backgroundPath;
        backgroundPath.addRoundedRect(backgroundRect, 4, 4);
        painter.fillPath(backgroundPath,
                         palette->color(StylePalette::Hover, StylePalette::Widget, StylePalette::Background));
    }

    int contentLeft = kMarginLeft + kCheckBoxWidth + kCheckBoxTextGap;

    // 图标
    QRect iconRect(contentLeft, (height() - kIconSize) / 2, kIconSize, kIconSize);
    QIcon icon = QIcon::fromTheme(m_entry.iconName);
    if (icon.isNull())
    {
        icon = QIcon::fromTheme(m_entry.isKeyboard ? "input-keyboard" : "ksvg-kcp-input-method");
    }
    if (!icon.isNull())
    {
        icon.paint(&painter, iconRect, Qt::AlignCenter,
                   m_alreadyEnabled ? QIcon::Disabled : QIcon::Normal);
    }

    QColor textColor = palette->color(group, StylePalette::Widget, StylePalette::Foreground);

    // 文本区域：右侧给语言标签留位
    QRect langRect = langLabelRect();
    int textRight = langRect.isValid() ? langRect.left() - kSpacing : width() - kMarginRight;
    int textLeft = iconRect.right() + 1 + kIconTextGap;
    int textWidth = qMax(0, textRight - textLeft);

    QString primary = primaryText();
    QString secondary = secondaryText();

    if (secondary.isEmpty() || secondary == primary)
    {
        painter.setPen(textColor);
        QFontMetrics fm(font());
        painter.drawText(QRect(textLeft, 0, textWidth, height()),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         fm.elidedText(primary, Qt::ElideRight, textWidth));
    }
    else
    {
        QFont primaryFont = font();
        QFontMetrics primaryFm(primaryFont);

        QFont secondaryFont = font();
        secondaryFont.setPointSizeF(qMax(6.0, font().pointSizeF() - 1));
        QFontMetrics secondaryFm(secondaryFont);

        int totalHeight = primaryFm.height() + secondaryFm.height();
        int top = (height() - totalHeight) / 2;

        painter.setFont(primaryFont);
        painter.setPen(textColor);
        painter.drawText(QRect(textLeft, top, textWidth, primaryFm.height()),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         primaryFm.elidedText(primary, Qt::ElideRight, textWidth));

        QColor secondaryColor = textColor;
        secondaryColor.setAlphaF(0.55);
        painter.setFont(secondaryFont);
        painter.setPen(secondaryColor);
        painter.drawText(QRect(textLeft, top + primaryFm.height(), textWidth, secondaryFm.height()),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         secondaryFm.elidedText(secondary, Qt::ElideRight, textWidth));
    }

    // 语言标签
    if (langRect.isValid())
    {
        QColor labelBackground = textColor;
        labelBackground.setAlphaF(0.10);

        QPainterPath labelPath;
        labelPath.addRoundedRect(QRectF(langRect), kLangLabelRadius, kLangLabelRadius);
        painter.fillPath(labelPath, labelBackground);

        QFont labelFont = font();
        labelFont.setPointSizeF(qMax(6.0, font().pointSizeF() - 1));
        painter.setFont(labelFont);

        QColor labelTextColor = textColor;
        labelTextColor.setAlphaF(0.70);
        painter.setPen(labelTextColor);
        painter.drawText(langRect, Qt::AlignCenter, m_entry.langCode);
    }
}
