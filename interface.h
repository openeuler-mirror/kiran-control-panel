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

extern bool gHasUnsavedOptions;

QString gName = "显示设置";
QList<SubItem> getSubitems()
{
    return QList<SubItem>() << SubItem(gName, "/usr/share/kiran-control-panel/plugins/icons/display-settings.svg", "");
}

QWidget *getSubitemWidget(QString name)
{
    gHasUnsavedOptions = false;
    if(gName == name)
    {
        return new KiranDisplayConfiguration();
    }

    return nullptr;
}

QString getTranslationPath()
{
    return "/usr/share/kiran-control-panel/module/kiran-display-tools";
}

bool hasUnsavedOptions()
{
    return gHasUnsavedOptions;
}

#endif // INTERFACE_H
