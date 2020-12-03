/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef INTERFACE_H
#define INTERFACE_H

#include <QDebug>
#include "kiran-control-panel-module-interface.h"
#include "kiran-display-configuration.h"

using namespace KiranControlPanelModuleInterface;

extern KiranDisplayModuleBase *gCurWidget;

QString gName = "显示设置";
QList<SubItem> getSubitems()
{
    return QList<SubItem>() << SubItem(gName, "/usr/share/kiran-control-panel/plugins/kiran-display-tools/icons/display-settings.svg", "");
}

QWidget *getSubitemWidget(QString name)
{
    if(gName == name)
    {
        auto widget = new KiranDisplayConfiguration();
        gCurWidget = widget;
        return widget;
    }

    return nullptr;
}

QString getTranslationPath()
{
    return "/usr/share/kiran-control-panel/plugins/kiran-display-tools/translate/kiran-display-tools";
}

bool hasUnsavedOptions()
{
    if(!gCurWidget) return false;
    return gCurWidget->hasUnsavedOptions();
}

#endif // INTERFACE_H
