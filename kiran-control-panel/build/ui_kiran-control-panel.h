/********************************************************************************
** Form generated from reading UI file 'kiran-control-panel.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KIRAN_2D_CONTROL_2D_PANEL_H
#define UI_KIRAN_2D_CONTROL_2D_PANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>
#include "kiran-module-widget/kiran-module-widget.h"

QT_BEGIN_NAMESPACE

class Ui_KiranControlPanel
{
public:
    QHBoxLayout *horizontalLayout;
    KiranModuleWidget *module_widget;

    void setupUi(QWidget *KiranControlPanel)
    {
        if (KiranControlPanel->objectName().isEmpty())
            KiranControlPanel->setObjectName(QStringLiteral("KiranControlPanel"));
        KiranControlPanel->resize(400, 300);
        KiranControlPanel->setStyleSheet(QLatin1String("QWidget:focus{\n"
"outline: none;\n"
"}\n"
"\n"
" QWidget {\n"
"background-color: rgb(34, 34, 34);\n"
"color: #ffffff;\n"
"}\n"
"\n"
"\n"
"QWidget:disabled {\n"
"      background-color: rgba(255,255,255,5);\n"
"	color: rgba(255,255,255,60);\n"
"  }\n"
" QAbstractScrollArea QScrollBar:vertical{border:0px;width: 0px;background-color:rgba(255,255,255,30);}"));
        horizontalLayout = new QHBoxLayout(KiranControlPanel);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        module_widget = new KiranModuleWidget(KiranControlPanel);
        module_widget->setObjectName(QStringLiteral("module_widget"));

        horizontalLayout->addWidget(module_widget);


        retranslateUi(KiranControlPanel);

        QMetaObject::connectSlotsByName(KiranControlPanel);
    } // setupUi

    void retranslateUi(QWidget *KiranControlPanel)
    {
        KiranControlPanel->setWindowTitle(QApplication::translate("KiranControlPanel", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class KiranControlPanel: public Ui_KiranControlPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KIRAN_2D_CONTROL_2D_PANEL_H
