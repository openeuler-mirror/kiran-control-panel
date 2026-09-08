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
#include "font-plugin.h"
#include "font-subitem.h"

FontPlugin::FontPlugin(QObject *parent)
    : QObject(parent)
{
}

FontPlugin::~FontPlugin()
{
}

int FontPlugin::init(KiranControlPanel::PanelInterface *interface)
{
    m_panelInterface = interface;
    m_subitem.reset(new FontSubItem());
    return 0;
}

void FontPlugin::uninit()
{
}

QVector<KiranControlPanel::SubItemPtr> FontPlugin::getSubItems()
{
    return {m_subitem};
}
