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

#include "im-settings.h"

#include "logging-category.h"

#include <QGSettings>

namespace
{
const QByteArray kPluginSchema("com.kylinsec.kiran.control-panel.plugin");
// QGSettings 以 camelCase 暴露 key（dconf 中为 only-current-language）
const QString kOnlyCurrentLanguageKey = QStringLiteral("onlyCurrentLanguage");
const bool kOnlyCurrentLanguageDefault = true;
}  // namespace

bool ImSettings::onlyCurrentLanguage()
{
    if (!QGSettings::isSchemaInstalled(kPluginSchema))
    {
        KLOG_WARNING(qLcInputMethod) << kPluginSchema << "is not installed, use default";
        return kOnlyCurrentLanguageDefault;
    }

    QGSettings settings(kPluginSchema);
    if (!settings.keys().contains(kOnlyCurrentLanguageKey))
    {
        KLOG_WARNING(qLcInputMethod) << kPluginSchema << "has no key" << kOnlyCurrentLanguageKey
                                     << ", use default";
        return kOnlyCurrentLanguageDefault;
    }

    return settings.get(kOnlyCurrentLanguageKey).toBool();
}

bool ImSettings::setOnlyCurrentLanguage(bool value)
{
    if (!QGSettings::isSchemaInstalled(kPluginSchema))
    {
        KLOG_WARNING(qLcInputMethod) << kPluginSchema << "is not installed, drop write";
        return false;
    }

    QGSettings settings(kPluginSchema);
    if (!settings.keys().contains(kOnlyCurrentLanguageKey))
    {
        KLOG_WARNING(qLcInputMethod) << kPluginSchema << "has no key" << kOnlyCurrentLanguageKey
                                     << ", drop write";
        return false;
    }

    settings.set(kOnlyCurrentLanguageKey, value);
    return true;
}
