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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BatterySettingsPage
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *comboBox;
    QComboBox *comboBox_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_4;
    QComboBox *comboBox_3;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_3;
    QHBoxLayout *horizontalLayout_6;
    QComboBox *comboBox_4;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_4;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *BatterySettingsPage)
    {
        if (BatterySettingsPage->objectName().isEmpty())
            BatterySettingsPage->setObjectName(QStringLiteral("BatterySettingsPage"));
        BatterySettingsPage->resize(824, 551);
        verticalLayout = new QVBoxLayout(BatterySettingsPage);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 24, -1, -1);
        label = new QLabel(BatterySettingsPage);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(24);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 10, -1, -1);
        comboBox = new QComboBox(BatterySettingsPage);
        comboBox->setObjectName(QStringLiteral("comboBox"));

        horizontalLayout_2->addWidget(comboBox);

        comboBox_2 = new QComboBox(BatterySettingsPage);
        comboBox_2->setObjectName(QStringLiteral("comboBox_2"));

        horizontalLayout_2->addWidget(comboBox_2);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(-1, 24, -1, -1);
        label_2 = new QLabel(BatterySettingsPage);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_3->addWidget(label_2);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(-1, 10, -1, -1);
        comboBox_3 = new QComboBox(BatterySettingsPage);
        comboBox_3->setObjectName(QStringLiteral("comboBox_3"));

        horizontalLayout_4->addWidget(comboBox_3);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(0);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(-1, 24, -1, -1);
        label_3 = new QLabel(BatterySettingsPage);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_5->addWidget(label_3);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(0);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(-1, 10, -1, -1);
        comboBox_4 = new QComboBox(BatterySettingsPage);
        comboBox_4->setObjectName(QStringLiteral("comboBox_4"));

        horizontalLayout_6->addWidget(comboBox_4);


        verticalLayout->addLayout(horizontalLayout_6);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(0);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(-1, 24, -1, -1);
        label_4 = new QLabel(BatterySettingsPage);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_7->addWidget(label_4);


        verticalLayout->addLayout(horizontalLayout_7);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(BatterySettingsPage);

        QMetaObject::connectSlotsByName(BatterySettingsPage);
    } // setupUi

    void retranslateUi(QWidget *BatterySettingsPage)
    {
        BatterySettingsPage->setWindowTitle(QApplication::translate("BatterySettingsPage", "BatterySettingsPage", nullptr));
        label->setText(QApplication::translate("BatterySettingsPage", "After idle for more than the following time, the computer will execute", nullptr));
        label_2->setText(QApplication::translate("BatterySettingsPage", "When the battery is lit up, it will be executed", nullptr));
        label_3->setText(QApplication::translate("BatterySettingsPage", "The monitor will turn off when it is idle", nullptr));
        label_4->setText(QApplication::translate("BatterySettingsPage", "Reduce screen brightness when idle", nullptr));
    } // retranslateUi

};

namespace Ui {
    class BatterySettingsPage: public Ui_BatterySettingsPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BATTERY_2D_SETTINGS_2D_PAGE_H
