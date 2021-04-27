/********************************************************************************
** Form generated from reading UI file 'power-settings-page.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POWER_2D_SETTINGS_2D_PAGE_H
#define UI_POWER_2D_SETTINGS_2D_PAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PowerSettingsPage
{
public:

    void setupUi(QWidget *PowerSettingsPage)
    {
        if (PowerSettingsPage->objectName().isEmpty())
            PowerSettingsPage->setObjectName(QStringLiteral("PowerSettingsPage"));
        PowerSettingsPage->resize(400, 300);

        retranslateUi(PowerSettingsPage);

        QMetaObject::connectSlotsByName(PowerSettingsPage);
    } // setupUi

    void retranslateUi(QWidget *PowerSettingsPage)
    {
        PowerSettingsPage->setWindowTitle(QApplication::translate("PowerSettingsPage", "PowerSettingsPage", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PowerSettingsPage: public Ui_PowerSettingsPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POWER_2D_SETTINGS_2D_PAGE_H
