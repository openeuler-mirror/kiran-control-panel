/********************************************************************************
** Form generated from reading UI file 'battery-settings-page.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BATTERY_2D_SETTINGS_2D_PAGE_H
#define UI_BATTERY_2D_SETTINGS_2D_PAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BatterySettingsPage
{
public:

    void setupUi(QWidget *BatterySettingsPage)
    {
        if (BatterySettingsPage->objectName().isEmpty())
            BatterySettingsPage->setObjectName(QStringLiteral("BatterySettingsPage"));
        BatterySettingsPage->resize(400, 300);

        retranslateUi(BatterySettingsPage);

        QMetaObject::connectSlotsByName(BatterySettingsPage);
    } // setupUi

    void retranslateUi(QWidget *BatterySettingsPage)
    {
        BatterySettingsPage->setWindowTitle(QApplication::translate("BatterySettingsPage", "BatterySettingsPage", nullptr));
    } // retranslateUi

};

namespace Ui {
    class BatterySettingsPage: public Ui_BatterySettingsPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BATTERY_2D_SETTINGS_2D_PAGE_H
