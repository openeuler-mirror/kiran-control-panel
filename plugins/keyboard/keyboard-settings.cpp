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

#include "keyboard-settings.h"

#include <kiran-log/qt5-log-i.h>
#include <QGSettings>

namespace
{
const QByteArray kPluginSchema("com.kylinsec.kiran.control-panel.plugin");
// QGSettings 以 camelCase 暴露 key（dconf 中为 enable-keyboard-layout-subitem）
const QString kEnableLayoutSubItemKey = QStringLiteral("enableKeyboardLayoutSubitem");
// 默认不提供「键盘布局」子项，键盘布局由输入法插件承担
const bool kEnableLayoutSubItemDefault = false;
}  // namespace

bool KeyboardSettings::enableLayoutSubItem()
{
    if (!QGSettings::isSchemaInstalled(kPluginSchema))
    {
        KLOG_WARNING() << kPluginSchema << "is not installed, use default";
        return kEnableLayoutSubItemDefault;
    }

    QGSettings settings(kPluginSchema);
    if (!settings.keys().contains(kEnableLayoutSubItemKey))
    {
        KLOG_WARNING() << kPluginSchema << "has no key" << kEnableLayoutSubItemKey << ", use default";
        return kEnableLayoutSubItemDefault;
    }

    return settings.get(kEnableLayoutSubItemKey).toBool();
}
