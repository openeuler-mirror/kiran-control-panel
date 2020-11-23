/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef KIRANCONTROLPANELMODULEINTERFACE_H
#define KIRANCONTROLPANELMODULEINTERFACE_H

#include <QWidget>
#include <QStringList>

extern "C" void getSubitems(QStringList &nameList, QStringList &iconList, QStringList &keyList);
extern "C" QWidget *getSubitemWidget(QString name);
extern "C" QString getTranslationPath();
extern "C" bool hasUnsavedOptions();

#endif // KIRANCONTROLPANELMODULEINTERFACE_H
