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
#include "printer-plugin.h"
#include "logging-category.h"
#include "printer-subitem.h"

#include <QCoreApplication>

Q_LOGGING_CATEGORY(qLcPrinter, "kcp.printer", QtMsgType::QtDebugMsg);

PrinterPlugin::PrinterPlugin(QObject *parent)
    : QObject(parent)
{
}

PrinterPlugin::~PrinterPlugin()
{
}

int PrinterPlugin::init(KiranControlPanel::PanelInterface *interface)
{
    Q_UNUSED(interface);

    m_subitem.reset(new PrinterSubItem());
    return 0;
}

void PrinterPlugin::uninit()
{
    m_subitem.reset();
}

QVector<KiranControlPanel::SubItemPtr> PrinterPlugin::getSubItems()
{
    return {m_subitem};
}
