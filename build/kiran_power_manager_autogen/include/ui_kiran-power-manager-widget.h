/********************************************************************************
** Form generated from reading UI file 'kiran-power-manager-widget.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KIRAN_2D_POWER_2D_MANAGER_2D_WIDGET_H
#define UI_KIRAN_2D_POWER_2D_MANAGER_2D_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <kiran-sidebar-widget.h>

QT_BEGIN_NAMESPACE

class Ui_KiranPowerManagerWidget
{
public:
    QHBoxLayout *horizontalLayout;
    KiranSidebarWidget *sidbar;
    QVBoxLayout *verticalLayout;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QWidget *page_2;

    void setupUi(QWidget *KiranPowerManagerWidget)
    {
        if (KiranPowerManagerWidget->objectName().isEmpty())
            KiranPowerManagerWidget->setObjectName(QStringLiteral("KiranPowerManagerWidget"));
        KiranPowerManagerWidget->resize(930, 597);
        horizontalLayout = new QHBoxLayout(KiranPowerManagerWidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        sidbar = new KiranSidebarWidget(KiranPowerManagerWidget);
        sidbar->setObjectName(QStringLiteral("sidbar"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(sidbar->sizePolicy().hasHeightForWidth());
        sidbar->setSizePolicy(sizePolicy);
        sidbar->setMinimumSize(QSize(300, 0));
        sidbar->setMaximumSize(QSize(300, 16777215));

        horizontalLayout->addWidget(sidbar);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        stackedWidget = new QStackedWidget(KiranPowerManagerWidget);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        page = new QWidget();
        page->setObjectName(QStringLiteral("page"));
        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName(QStringLiteral("page_2"));
        stackedWidget->addWidget(page_2);

        verticalLayout->addWidget(stackedWidget);


        horizontalLayout->addLayout(verticalLayout);


        retranslateUi(KiranPowerManagerWidget);

        QMetaObject::connectSlotsByName(KiranPowerManagerWidget);
    } // setupUi

    void retranslateUi(QWidget *KiranPowerManagerWidget)
    {
        KiranPowerManagerWidget->setWindowTitle(QApplication::translate("KiranPowerManagerWidget", "KiranPowerManagerWidget", nullptr));
    } // retranslateUi

};

namespace Ui {
    class KiranPowerManagerWidget: public Ui_KiranPowerManagerWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KIRAN_2D_POWER_2D_MANAGER_2D_WIDGET_H
