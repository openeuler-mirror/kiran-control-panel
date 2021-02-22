/**
 * @file kiran-control-center-single-plugin.cpp
 * @brief description
 * @author yangxiaoqing <yangxiaoqing@kylinos.com.cn>
 * @copyright (c) 2021 KylinSec. All rights reserved.
*/
#include "kiran-control-center-single-plugin.h"
#include "kiran-module-widget/kiran-module-widget.h"
#include <QIcon>
#include <QDebug>

KiranControlCenterSinglePlugin::KiranControlCenterSinglePlugin() :
    KiranTitlebarWindow()
{
    setButtonHints(KiranTitlebarWindow::TitlebarMinimizeButtonHint|KiranTitlebarWindow::TitlebarMaximizeButtonHint|KiranTitlebarWindow::TitlebarCloseButtonHint);
    resize(708, 830);
}

KiranControlCenterSinglePlugin::~KiranControlCenterSinglePlugin()
{

}

void KiranControlCenterSinglePlugin::setModuleName(const QString &name)
{
    ModuleItem data = getModuleItem(name);
    m_map.insert(1, data);

    setIcon(QIcon(data.icon));
    setTitle(data.getNameTranslate());
    KiranModuleWidget *centerWgt = new KiranModuleWidget(this);
    centerWgt->setData(&m_map);
    setWindowContentWidget(centerWgt);
}
