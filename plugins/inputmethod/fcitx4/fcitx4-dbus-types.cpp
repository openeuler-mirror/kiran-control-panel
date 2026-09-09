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

#include "fcitx4-dbus-types.h"

#include <QDBusArgument>

QDBusArgument& operator<<(QDBusArgument& arg, const Fcitx4ImItem& item)
{
    arg.beginStructure();
    arg << item.name << item.uniqueName << item.langCode << item.enabled;
    arg.endStructure();
    return arg;
}

const QDBusArgument& operator>>(const QDBusArgument& arg, Fcitx4ImItem& item)
{
    arg.beginStructure();
    arg >> item.name >> item.uniqueName >> item.langCode >> item.enabled;
    arg.endStructure();
    return arg;
}
