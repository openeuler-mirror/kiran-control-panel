/**
 * Copyright (c) 2020 ~ 2024 KylinSec Co., Ltd.
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
#include "power-prefs.h"

#include <kiran-log/qt5-log-i.h>
#include <QGSettings>

namespace Power
{
namespace
{
const QByteArray kPluginSchema("com.kylinsec.kiran.control-panel.plugin");
// QGSettings 以 camelCase 暴露 key（dconf 中为 server-mode）
const QString kServerModeKey = QStringLiteral("serverMode");
// 默认桌面模式，服务器版由打包时安装的 gschema override 打开
const bool kServerModeDefault = false;
}  // namespace

Prefs::Prefs()
{
    if (!QGSettings::isSchemaInstalled(kPluginSchema))
    {
        KLOG_WARNING() << kPluginSchema << "is not installed, use default server mode";
        return;
    }

    m_settings = new QGSettings(kPluginSchema);
}

Prefs::~Prefs()
{
    delete m_settings;
    m_settings = nullptr;
}

bool Prefs::enableServerMode() const
{
    if (!m_settings || !m_settings->keys().contains(kServerModeKey))
    {
        return kServerModeDefault;
    }

    return m_settings->get(kServerModeKey).toBool();
}
}  // namespace Power
