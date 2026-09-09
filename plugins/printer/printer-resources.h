/**
 * Copyright (c) 2026 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     gaobo <gaobo@kylinsec.com.cn>
 */
#pragma once

#include <QPixmap>
#include <QString>

namespace PrinterResources
{
inline QString onlinePath()
{
    return QStringLiteral(":/kcp-printer/images/printer-online.png");
}

inline QString offlinePath()
{
    return QStringLiteral(":/kcp-printer/images/printer-offline.png");
}

inline QString starPath()
{
    return QStringLiteral(":/kcp-printer/images/star.svg");
}

inline QString warningPath()
{
    return QStringLiteral(":/kcp-printer/images/cups-service-warning.png");
}

inline QPixmap printerItemPixmap(bool online)
{
    QPixmap pix(online ? onlinePath() : offlinePath());
    return pix.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}
}  // namespace PrinterResources
