/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-group is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan
 * PSL v2. You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
 * Mulan PSL v2 for more details.
 *
 * Author:     wangshichang <shichang@isrc.iscas.ac.cn>
 */

#include "group-plugin.h"
#include "group-subitem.h"

#include <qt5-log-i.h>

GroupPlugin::GroupPlugin(QObject *parent)
    : QObject(parent)
{
}

GroupPlugin::~GroupPlugin()
{
}

int GroupPlugin::init(KiranControlPanel::PanelInterface *interface)
{
    m_panelInterface = interface;
    m_subitem.reset(new GroupSubItem());
    return 0;
}

void GroupPlugin::uninit()
{
}

QVector<KiranControlPanel::SubItemPtr> GroupPlugin::getSubItems()
{
    return {m_subitem};
}
