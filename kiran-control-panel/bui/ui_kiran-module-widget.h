/********************************************************************************
** Form generated from reading UI file 'kiran-module-widget.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KIRAN_2D_MODULE_2D_WIDGET_H
#define UI_KIRAN_2D_MODULE_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_KiranModuleWidget
{
public:
    QHBoxLayout *horizontalLayout;
    QListWidget *listWidget_module;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *centerLayout;

    void setupUi(QWidget *KiranModuleWidget)
    {
        if (KiranModuleWidget->objectName().isEmpty())
            KiranModuleWidget->setObjectName(QStringLiteral("KiranModuleWidget"));
        KiranModuleWidget->resize(400, 300);
        KiranModuleWidget->setStyleSheet(QLatin1String("QWidget:focus{\n"
"outline: none;\n"
"}\n"
"\n"
" QWidget {\n"
"background-color:#222222;\n"
"color: #ffffff;\n"
"}\n"
"\n"
"\n"
"QWidget:disabled {\n"
"      background-color: rgba(255,255,255,5);\n"
"	color: rgba(255,255,255,60);\n"
"  }\n"
""));
        horizontalLayout = new QHBoxLayout(KiranModuleWidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        listWidget_module = new QListWidget(KiranModuleWidget);
        listWidget_module->setObjectName(QStringLiteral("listWidget_module"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(listWidget_module->sizePolicy().hasHeightForWidth());
        listWidget_module->setSizePolicy(sizePolicy);
        listWidget_module->setMinimumSize(QSize(330, 0));
        listWidget_module->setMaximumSize(QSize(330, 16777215));
        listWidget_module->setStyleSheet(QLatin1String("QListWidget{\n"
"	border-left: 0px solid rgba(255, 255, 255, 10);\n"
"	border-top: 0px solid rgba(255, 255, 255, 10);\n"
"	border-right: 1px solid rgba(255, 255, 255, 10);\n"
"	border-bottom: 0px solid rgba(255, 255, 255, 10);\n"
"	padding-left:12px;\n"
"	padding-right:12px;\n"
" 	padding-top:10px;\n"
"	padding-bottom:10px;\n"
"}\n"
"\n"
"\n"
"  QListWidget::item {\n"
"    border: 0px solid #2eb3ff;\n"
"	color: #ffffff;	\n"
"	border-radius: 10px;\n"
"	background-color: rgba(255, 255, 255, 5);\n"
"	padding-left:14px;\n"
"  }\n"
"\n"
"  QListWidget::item:hover {\n"
"	background-color: rgba(255, 255, 255, 10);\n"
"  }\n"
"  QListWidget::item:selected {\n"
"      background-color:#43a3f2;\n"
"  }\n"
""));

        horizontalLayout->addWidget(listWidget_module);

        widget = new QWidget(KiranModuleWidget);
        widget->setObjectName(QStringLiteral("widget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy1);
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        centerLayout = new QHBoxLayout();
        centerLayout->setObjectName(QStringLiteral("centerLayout"));

        verticalLayout->addLayout(centerLayout);


        horizontalLayout->addWidget(widget);


        retranslateUi(KiranModuleWidget);

        QMetaObject::connectSlotsByName(KiranModuleWidget);
    } // setupUi

    void retranslateUi(QWidget *KiranModuleWidget)
    {
        KiranModuleWidget->setWindowTitle(QApplication::translate("KiranModuleWidget", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class KiranModuleWidget: public Ui_KiranModuleWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KIRAN_2D_MODULE_2D_WIDGET_H
