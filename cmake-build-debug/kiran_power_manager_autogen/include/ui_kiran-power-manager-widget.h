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
    QWidget *siderbarWidget;
    QVBoxLayout *verticalLayout_2;
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
        siderbarWidget = new QWidget(KiranPowerManagerWidget);
        siderbarWidget->setObjectName(QStringLiteral("siderbarWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(siderbarWidget->sizePolicy().hasHeightForWidth());
        siderbarWidget->setSizePolicy(sizePolicy);
        siderbarWidget->setMinimumSize(QSize(300, 0));
        siderbarWidget->setMaximumSize(QSize(300, 16777215));
        verticalLayout_2 = new QVBoxLayout(siderbarWidget);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 2, 0);
        sidbar = new KiranSidebarWidget(siderbarWidget);
        sidbar->setObjectName(QStringLiteral("sidbar"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(sidbar->sizePolicy().hasHeightForWidth());
        sidbar->setSizePolicy(sizePolicy1);
        sidbar->setMinimumSize(QSize(0, 0));
        sidbar->setMaximumSize(QSize(16777215, 16777215));

        verticalLayout_2->addWidget(sidbar);


        horizontalLayout->addWidget(siderbarWidget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(24, -1, 24, -1);
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
