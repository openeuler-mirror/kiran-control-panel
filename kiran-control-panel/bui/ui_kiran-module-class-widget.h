/********************************************************************************
** Form generated from reading UI file 'kiran-module-class-widget.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KIRAN_2D_MODULE_2D_CLASS_2D_WIDGET_H
#define UI_KIRAN_2D_MODULE_2D_CLASS_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>
#include "kiran-module-class-listwidget.h"

QT_BEGIN_NAMESPACE

class Ui_KiranModuleClassWidget
{
public:
    QHBoxLayout *horizontalLayout;
    KiranModuleClassListWidget *list_widget;

    void setupUi(QWidget *KiranModuleClassWidget)
    {
        if (KiranModuleClassWidget->objectName().isEmpty())
            KiranModuleClassWidget->setObjectName(QStringLiteral("KiranModuleClassWidget"));
        KiranModuleClassWidget->resize(400, 300);
        KiranModuleClassWidget->setStyleSheet(QLatin1String("QScrollBar:vertical {\n"
"      border: 0px solid grey;\n"
"      background: rgba(255,255,255,10);\n"
"      width: 6px;\n"
"      margin: 20px 0px 20px 0px;\n"
"  }\n"
"  QScrollBar::handle:vertical {\n"
"      background: rgba(255,255,255,30);\n"
"      min-height: 20px;\n"
"  }\n"
"  QScrollBar::add-line:vertical {\n"
"      border: 0px solid grey;\n"
"      background: #32CC99;\n"
"      height: 0px;\n"
"      subcontrol-position: right;\n"
"      subcontrol-origin: margin;\n"
"  }\n"
"\n"
"  QScrollBar::sub-line:vertical {\n"
"      border: 0px solid grey;\n"
"      background: #32CC99;\n"
"      height: 0px;\n"
"      subcontrol-position: left;\n"
"      subcontrol-origin: margin;\n"
"  }\n"
"\n"
"  QScrollBar:horizontal {\n"
"      border: 0px solid grey;\n"
"      background: rgba(255,255,255,10);\n"
"      height: 6px;\n"
"      margin: 0px 20px 0 20px;\n"
"  }\n"
"  QScrollBar::handle:horizontal {\n"
"      background: rgba(255,255,255,30);\n"
"      min-width: 20px;\n"
"  }\n"
"  QScrollBar::add-li"
                        "ne:horizontal {\n"
"      border: 0px solid grey;\n"
"      width: 0px;\n"
"      subcontrol-position: right;\n"
"      subcontrol-origin: margin;\n"
"  }\n"
"\n"
"  QScrollBar::sub-line:horizontal {\n"
"      border: 0px solid grey;\n"
"      width: 0px;\n"
"      subcontrol-position: left;\n"
"      subcontrol-origin: margin;\n"
"  }"));
        horizontalLayout = new QHBoxLayout(KiranModuleClassWidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        list_widget = new KiranModuleClassListWidget(KiranModuleClassWidget);
        list_widget->setObjectName(QStringLiteral("list_widget"));

        horizontalLayout->addWidget(list_widget);


        retranslateUi(KiranModuleClassWidget);

        QMetaObject::connectSlotsByName(KiranModuleClassWidget);
    } // setupUi

    void retranslateUi(QWidget *KiranModuleClassWidget)
    {
        KiranModuleClassWidget->setWindowTitle(QApplication::translate("KiranModuleClassWidget", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class KiranModuleClassWidget: public Ui_KiranModuleClassWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KIRAN_2D_MODULE_2D_CLASS_2D_WIDGET_H
