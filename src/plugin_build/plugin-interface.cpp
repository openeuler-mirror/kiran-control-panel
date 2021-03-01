//
// Created by lxh on 2021/3/1.
//

#include "plugin-interface.h"
#include "kiran-timedate-widget.h"
#include "kiran-timedate-global-data.h"
#include "config.h"

#include <QMessageBox>
#include <QDebug>

using namespace ModuleInterface;
static const char* dateTimeItemName = "DateTime";

QList<ModuleInterface::SubItem> getSubitems() {
    return QList<SubItem>() << SubItem(dateTimeItemName,QString(),"dateTime");
}

QWidget *getSubitemWidget(QString widgetName) {
    QWidget* widgetPtr = nullptr;
    if(widgetName==dateTimeItemName){
        ///加载数据
        KiranTimeDateGlobalData::instance()->init();
        widgetPtr = new KiranTimeDateWidget;
    }
    return widgetPtr;
}

QString getTranslationPath() {
    return KIRAN_TIMEDATE_MANAGER_TRANSLATE_PATH;
}

bool hasUnsavedOptions() {
    return false;
}
