/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     yangxiaoqing <yangxiaoqing@kylinos.com.cn>
 */


#include "launcher.h"
#include "kiran-module-widget/kiran-module-widget.h"

#include <qt5-log-i.h>
#include <QApplication>
#include <QDebug>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QIcon>
#include <QScreen>

Launcher::Launcher(QWidget *parent) :
    KiranTitlebarWindow(parent)
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinMaxCloseHints);
    m_moduleWidget = new KiranModuleWidget(this);

    setWindowContentWidget(m_moduleWidget);
    installEventFilter(this);
}

Launcher::~Launcher()
{

}

void Launcher::setPlugins(const QList<QSharedPointer<CPanelPluginHelper>> &plugins)
{
    m_moduleWidget->setPlugins(plugins);
}

void Launcher::setPlugin(QSharedPointer<CPanelPluginHelper> plugin)
{
    QList<QSharedPointer<CPanelPluginHelper>> plugins = {plugin};
    m_moduleWidget->setPlugins(plugins);
}

QSize Launcher::sizeHint() const
{
    return KiranTitlebarWindow::sizeHint();
}
