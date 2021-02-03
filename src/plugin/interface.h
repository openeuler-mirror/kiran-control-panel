/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef INTERFACE_H
#define INTERFACE_H

#include <QDebug>
#include "module-interface.h"
#include "kiran-display-configuration.h"
#include "kiranwidgets-qt5/kiran-message-box.h"
#include <QDBusInterface>
#include <QApplication>
#include <QPushButton>

using namespace ModuleInterface;

extern KiranDisplayModuleBase *gCurWidget;

QString gName = "显示设置";
QList<SubItem> getSubitems()
{
    return QList<SubItem>() << SubItem(gName, QString(PLUGIN_ITME_ICON_DIR) + "display-settings.svg", "");
}

QWidget *getSubitemWidget(QString name)
{
    if(gName == name)
    {
        if(!QDBusInterface(KIRAN_DBUS_SERVICE_NAME, KIRAN_DBUS_DISPLAY).isValid())
        {
            KiranMessageBox box(qApp->allWidgets().isEmpty() ? nullptr : qApp->allWidgets().first());
            box.setTitle(QObject::tr("Tips"));

            QPushButton btn;
            btn.setText(QObject::tr("OK(K)"));
            btn.setFixedSize(QSize(200, box.buttonSize().height()));
            btn.setShortcut(Qt::CTRL + Qt::Key_K);
            box.addButton(&btn, QDialogButtonBox::AcceptRole);
            box.setText(QObject::tr("Background D-Bus service failed to connect, display setting failed to start, please check if D-Bus service is start."));
            box.exec();
            return nullptr;
        }

        auto widget = new KiranDisplayConfiguration();
        gCurWidget = widget;
        return widget;
    }

    return nullptr;
}

QString getTranslationPath()
{
    return QString(TRANSLATE_PATH_PREFIX);
}

bool hasUnsavedOptions()
{
    if(!gCurWidget) return false;
    return gCurWidget->hasUnsavedOptions();
}

#endif // INTERFACE_H
