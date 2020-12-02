/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef INTERFACE_H
#define INTERFACE_H

#include <QDebug>
#include "../../interface/kiran-control-panel-module-interface.h"
#include "dialog001.h"
#include "dialog002.h"

using namespace KiranControlPanelModuleInterface;
extern bool gHasUnsavedOptions;

QList<SubItem> getSubitems()
{
    return QList<SubItem>() << SubItem("dialog001", "avatar-default", "001") << SubItem("dialog002", "avatar-default", "002");
}

QWidget *getSubitemWidget(QString name)
{
    gHasUnsavedOptions = false;
    if("dialog001" == name)
    {
        return new Dialog001();
    }
    else if("dialog002" == name)
    {
        return new Dialog002();
    }

    return nullptr;
}

QString getTranslationPath()
{


    return "/root/kiran-control-panel/example/modeule01/";
}

bool hasUnsavedOptions()
{
    return gHasUnsavedOptions;
}

#endif // INTERFACE_H
