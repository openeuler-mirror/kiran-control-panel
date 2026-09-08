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
#include "font-subitem.h"
#include "font-page.h"

#include <QEvent>

FontSubItem::FontSubItem(QObject *parent)
    : QObject(parent)
{
}

FontSubItem::~FontSubItem()
{
}

bool FontSubItem::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_fontPage && event->type() == QEvent::Destroy)
    {
        m_fontPage = nullptr;
    }

    return QObject::eventFilter(watched, event);
}

QString FontSubItem::getID()
{
    return "FontManagement";
}

QString FontSubItem::getName()
{
    return tr("Font Management");
}

QString FontSubItem::getCategory()
{
    return "individuation";
}

QString FontSubItem::getDesc()
{
    return "";
}

QString FontSubItem::getIcon()
{
    return "ksvg-kcp-font";
}

int FontSubItem::getWeight()
{
    return 96;
}

QWidget *FontSubItem::createWidget()
{
    m_fontPage = new FontPage();
    m_fontPage->installEventFilter(this);
    return m_fontPage;
}

QVector<QPair<QString, QString>> FontSubItem::getSearchKeys()
{
    return {};
}

bool FontSubItem::jumpToSearchEntry(const QString &key)
{
    return false;
}
