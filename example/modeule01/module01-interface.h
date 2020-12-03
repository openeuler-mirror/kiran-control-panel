/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef INTERFACE_H
#define INTERFACE_H

#include <QDebug>
#include "kiran-control-panel-module-interface.h"
#include "dialog001.h"
#include "dialog002.h"

using namespace KiranControlPanelModuleInterface;
extern bool gHasUnsavedOptions;

QString gName1 = "dialog001";
QString gName2 = "dialog002";
QList<SubItem> getSubitems()
{
    return QList<SubItem>() << SubItem(gName1, "/usr/share/kiran-control-panel/plugins/icons/avatar-default.svg", "001")
                            << SubItem(gName2, "/usr/share/kiran-control-panel/plugins/icons/avatar-default.svg", "002");
}

QWidget *getSubitemWidget(QString name)
{
    gHasUnsavedOptions = false;
    if(gName1 == name)
    {
        return new Dialog001();
    }
    else if(gName2 == name)
    {
        return new Dialog002();
    }

    return nullptr;
}

QString getTranslationPath()
{
    return "/usr/share/kiran-control-panel/plugins/translate/module01";
}

bool hasUnsavedOptions()
{
    return gHasUnsavedOptions;
}

#endif // INTERFACE_H
