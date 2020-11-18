/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef INTERFACE_H
#define INTERFACE_H
#include <QDebug>

#include "dialog001.h"
#include "dialog002.h"
#include "dialog003.h"

extern "C" void get_subitems(QStringList &nameList, QStringList &iconList, QStringList &keyList);
extern "C" QWidget *get_subitem_widget(QString name);
extern "C" QString get_translation_path();
extern "C" bool has_unsaved_options();
extern modelbase *gCurWgt;

void get_subitems(QStringList &nameList, QStringList &iconList, QStringList &keyList)
{
    nameList << "dialog001" << "dialog002" << "dialog003" ;
    iconList << "./icon1.svg"<< "./icon2.svg" << "./icon2.svg";
    keyList << "001" << "002" << "003";
}

QWidget *get_subitem_widget(QString name)
{
    if("dialog001" == name)
    {
        auto wgt = new Dialog001();
        gCurWgt = wgt;
        return wgt;
    }
    else if("dialog002" == name)
    {
        auto wgt = new Dialog002();
        gCurWgt = wgt;
        return wgt;
    }
    else if("dialog003" == name)
    {
        auto wgt = new Dialog003();
        gCurWgt = wgt;
        return wgt;
    }

    return nullptr;
}

QString get_translation_path()
{
    return "/usr/share/xx/";
}

bool has_unsaved_options()
{
    if(!gCurWgt) return false;
    return gCurWgt->has_unsaved_options();
}

#endif // INTERFACE_H
