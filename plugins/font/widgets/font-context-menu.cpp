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
#include "font-context-menu.h"

#include <palette.h>

#include <QPainter>
#include <QStyle>
#include <QStyleOptionMenuItem>
#include <QtGlobal>

using namespace Kiran::Theme;
FontContextMenu::FontContextMenu(QWidget *parent)
    : QMenu(parent)
{
}

void FontContextMenu::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QColor disabledColor = DEFAULT_PALETTE()->getColor(Palette::DISABLED, Palette::TEXT);

    Q_FOREACH (QAction *action, actions())
    {
        const QRect actionRect = actionGeometry(action);
        if (!event->rect().intersects(actionRect))
            continue;

        QStyleOptionMenuItem opt;
        initStyleOption(&opt, action);
        opt.rect = actionRect;

        if (!action->isEnabled() && !action->isSeparator())
        {
            // 走启用绘制路径，避免 SH_EtchDisabledText 蚀刻叠画；文字改为灰色。
            opt.state |= QStyle::State_Enabled;
            opt.palette.setColor(QPalette::Text, disabledColor);
            opt.palette.setColor(QPalette::WindowText, disabledColor);
            opt.palette.setColor(QPalette::ButtonText, disabledColor);
            if (!opt.icon.isNull())
            {
                const QSize iconSize = opt.icon.actualSize(QSize(opt.rect.height(), opt.rect.height()),
                                                           QIcon::Disabled);
                QIcon grayIcon;
                grayIcon.addPixmap(opt.icon.pixmap(iconSize, QIcon::Disabled));
                opt.icon = grayIcon;
            }
        }

        style()->drawControl(QStyle::CE_MenuItem, &opt, &painter, this);
    }
}
