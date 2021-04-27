/********************************************************************************
** Form generated from reading UI file 'kiran-power-manager-window.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KIRAN_2D_POWER_2D_MANAGER_2D_WINDOW_H
#define UI_KIRAN_2D_POWER_2D_MANAGER_2D_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_KiranPowerManagerWindow
{
public:

    void setupUi(QWidget *KiranPowerManagerWindow)
    {
        if (KiranPowerManagerWindow->objectName().isEmpty())
            KiranPowerManagerWindow->setObjectName(QStringLiteral("KiranPowerManagerWindow"));
        KiranPowerManagerWindow->resize(400, 300);

        retranslateUi(KiranPowerManagerWindow);

        QMetaObject::connectSlotsByName(KiranPowerManagerWindow);
    } // setupUi

    void retranslateUi(QWidget *KiranPowerManagerWindow)
    {
        KiranPowerManagerWindow->setWindowTitle(QApplication::translate("KiranPowerManagerWindow", "KiranPowerManagerWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class KiranPowerManagerWindow: public Ui_KiranPowerManagerWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KIRAN_2D_POWER_2D_MANAGER_2D_WINDOW_H
