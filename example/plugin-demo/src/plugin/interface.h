/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef INTERFACE_H
#define INTERFACE_H

#include "module-interface.h"
#include "dialog001.h"
#include "dialog002.h"
#include "kiranwidgets-qt5/kiran-message-box.h"
#include "config.h"

using namespace ModuleInterface;

extern PluginSubItemBase *gCurWidget;
QString gName1 = "dialog001";
QString gName2 = "dialog002";
QList<SubItem> getSubitems()
{
    return QList<SubItem>() << SubItem(gName1, QString(PLUGIN_SUB_ITME_ICON_DIR) + "dialog001.svg", "")
                            << SubItem(gName2, QString(PLUGIN_SUB_ITME_ICON_DIR) + "dialog002.svg", "");
}

QWidget *getSubitemWidget(QString name)
{
    if(gName1 == name)
    {
        auto widget = new Dialog001();
        gCurWidget = widget;
        return widget;
    }
    else if(gName2 == name)
    {
        auto widget = new Dialog002();
        gCurWidget = widget;
        return widget;
    }

    return nullptr;
}

QString getTranslationPath()
{
    return QString(PLUGIN_SUB_ITME_TRANSLATE_DIR_PREFIX);
}

bool hasUnsavedOptions()
{
    if(!gCurWidget) return false;
    return gCurWidget->hasUnsavedOptions();
}

#endif // INTERFACE_H
