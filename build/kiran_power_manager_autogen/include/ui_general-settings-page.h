/********************************************************************************
** Form generated from reading UI file 'general-settings-page.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GENERAL_2D_SETTINGS_2D_PAGE_H
#define UI_GENERAL_2D_SETTINGS_2D_PAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GeneralSettingsPage
{
public:

    void setupUi(QWidget *GeneralSettingsPage)
    {
        if (GeneralSettingsPage->objectName().isEmpty())
            GeneralSettingsPage->setObjectName(QStringLiteral("GeneralSettingsPage"));
        GeneralSettingsPage->resize(400, 300);

        retranslateUi(GeneralSettingsPage);

        QMetaObject::connectSlotsByName(GeneralSettingsPage);
    } // setupUi

    void retranslateUi(QWidget *GeneralSettingsPage)
    {
        GeneralSettingsPage->setWindowTitle(QApplication::translate("GeneralSettingsPage", "GeneralSettingsPage", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GeneralSettingsPage: public Ui_GeneralSettingsPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GENERAL_2D_SETTINGS_2D_PAGE_H
