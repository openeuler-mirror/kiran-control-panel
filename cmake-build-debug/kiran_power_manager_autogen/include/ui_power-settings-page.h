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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PowerSettingsPage
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *combo_idleTime;
    QComboBox *combo_idleAction;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_4;
    QComboBox *combo_displayOffTime;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *PowerSettingsPage)
    {
        if (PowerSettingsPage->objectName().isEmpty())
            PowerSettingsPage->setObjectName(QStringLiteral("PowerSettingsPage"));
        PowerSettingsPage->resize(736, 562);
        verticalLayout = new QVBoxLayout(PowerSettingsPage);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 24, -1, -1);
        label = new QLabel(PowerSettingsPage);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(24);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 10, -1, -1);
        combo_idleTime = new QComboBox(PowerSettingsPage);
        combo_idleTime->setObjectName(QStringLiteral("combo_idleTime"));

        horizontalLayout_2->addWidget(combo_idleTime);

        combo_idleAction = new QComboBox(PowerSettingsPage);
        combo_idleAction->setObjectName(QStringLiteral("combo_idleAction"));

        horizontalLayout_2->addWidget(combo_idleAction);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(-1, 24, -1, -1);
        label_2 = new QLabel(PowerSettingsPage);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_3->addWidget(label_2);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(-1, 10, -1, -1);
        combo_displayOffTime = new QComboBox(PowerSettingsPage);
        combo_displayOffTime->setObjectName(QStringLiteral("combo_displayOffTime"));

        horizontalLayout_4->addWidget(combo_displayOffTime);


        verticalLayout->addLayout(horizontalLayout_4);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(PowerSettingsPage);

        QMetaObject::connectSlotsByName(PowerSettingsPage);
    } // setupUi

    void retranslateUi(QWidget *PowerSettingsPage)
    {
        PowerSettingsPage->setWindowTitle(QApplication::translate("PowerSettingsPage", "PowerSettingsPage", nullptr));
        label->setText(QApplication::translate("PowerSettingsPage", "After idle for more than the following time, the computer will execute", nullptr));
        label_2->setText(QApplication::translate("PowerSettingsPage", "The monitor will turn off when it is idle", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PowerSettingsPage: public Ui_PowerSettingsPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POWER_2D_SETTINGS_2D_PAGE_H
