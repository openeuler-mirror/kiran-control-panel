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

void getSubitems(QStringList &nameList, QStringList &iconList, QStringList &keyList)
{
    nameList << "dialog001"   << "dialog002";
    iconList << "./icon1.svg" << "./icon2.svg";
    keyList  << "001"         << "002" ;
}

QWidget *getSubitemWidget(QString name)
{
    qDebug() << name;
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
    return "/usr/share/xx/";
}

bool hasUnsavedOptions()
{
    return true;
}

#endif // INTERFACE_H
