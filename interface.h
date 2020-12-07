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
#include "kiranwidgets-qt5/kiran-message-box.h"
#include <QDBusInterface>
#include <QApplication>
#include <QPushButton>

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
        if(!QDBusInterface(KIRAN_DBUS_SERVICE_NAME, KIRAN_DBUS_DISPLAY).isValid())
        {
            KiranMessageBox box(qApp->allWidgets().isEmpty() ? nullptr : qApp->allWidgets().first());
            box.setTitle(QObject::tr("提示"));

            QPushButton btn;
            btn.setText(QObject::tr("确定(K)"));
            btn.setFixedSize(QSize(200, box.buttonSize().height()));
            btn.setShortcut(Qt::CTRL + Qt::Key_K);
            box.addButton(&btn, QDialogButtonBox::AcceptRole);
            box.setText(QObject::tr("后台D-Bus服务无法连接，程序启动失败，请检查D-Bus服务是否开启."));
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
    return "/usr/share/kiran-control-panel/plugins/kiran-display-tools/translate/kiran-display-tools";
}

bool hasUnsavedOptions()
{
    if(!gCurWidget) return false;
    return gCurWidget->hasUnsavedOptions();
}

#endif // INTERFACE_H
