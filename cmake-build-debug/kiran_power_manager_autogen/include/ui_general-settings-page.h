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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GeneralSettingsPage
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *combo_powerButton;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_4;
    QComboBox *combo_suspendButton;
    QWidget *widget_lidLabel;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_3;
    QWidget *widget_lidCombo;
    QHBoxLayout *horizontalLayout_6;
    QComboBox *combo_closingLid;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_brightness;
    QSpacerItem *horizontalSpacer;
    QLabel *label_brightnessPercent;
    QHBoxLayout *horizontalLayout_9;
    QSlider *slider_brightness;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *GeneralSettingsPage)
    {
        if (GeneralSettingsPage->objectName().isEmpty())
            GeneralSettingsPage->setObjectName(QStringLiteral("GeneralSettingsPage"));
        GeneralSettingsPage->resize(772, 592);
        verticalLayout = new QVBoxLayout(GeneralSettingsPage);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 24, -1, -1);
        label = new QLabel(GeneralSettingsPage);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, 10, -1, -1);
        combo_powerButton = new QComboBox(GeneralSettingsPage);
        combo_powerButton->setObjectName(QStringLiteral("combo_powerButton"));

        horizontalLayout_2->addWidget(combo_powerButton);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(-1, 24, -1, -1);
        label_2 = new QLabel(GeneralSettingsPage);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_3->addWidget(label_2);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(-1, 10, -1, -1);
        combo_suspendButton = new QComboBox(GeneralSettingsPage);
        combo_suspendButton->setObjectName(QStringLiteral("combo_suspendButton"));

        horizontalLayout_4->addWidget(combo_suspendButton);


        verticalLayout->addLayout(horizontalLayout_4);

        widget_lidLabel = new QWidget(GeneralSettingsPage);
        widget_lidLabel->setObjectName(QStringLiteral("widget_lidLabel"));
        horizontalLayout_5 = new QHBoxLayout(widget_lidLabel);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(-1, 24, -1, -1);
        label_3 = new QLabel(widget_lidLabel);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_5->addWidget(label_3);


        verticalLayout->addWidget(widget_lidLabel);

        widget_lidCombo = new QWidget(GeneralSettingsPage);
        widget_lidCombo->setObjectName(QStringLiteral("widget_lidCombo"));
        widget_lidCombo->setAutoFillBackground(false);
        horizontalLayout_6 = new QHBoxLayout(widget_lidCombo);
        horizontalLayout_6->setSpacing(0);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(0, 10, 0, 0);
        combo_closingLid = new QComboBox(widget_lidCombo);
        combo_closingLid->setObjectName(QStringLiteral("combo_closingLid"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(combo_closingLid->sizePolicy().hasHeightForWidth());
        combo_closingLid->setSizePolicy(sizePolicy);
        combo_closingLid->setMinimumSize(QSize(0, 40));
        combo_closingLid->setMaximumSize(QSize(16777215, 40));

        horizontalLayout_6->addWidget(combo_closingLid);


        verticalLayout->addWidget(widget_lidCombo);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(-1, 24, -1, -1);
        label_brightness = new QLabel(GeneralSettingsPage);
        label_brightness->setObjectName(QStringLiteral("label_brightness"));

        horizontalLayout_7->addWidget(label_brightness);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer);

        label_brightnessPercent = new QLabel(GeneralSettingsPage);
        label_brightnessPercent->setObjectName(QStringLiteral("label_brightnessPercent"));

        horizontalLayout_7->addWidget(label_brightnessPercent);


        verticalLayout->addLayout(horizontalLayout_7);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        horizontalLayout_9->setContentsMargins(-1, 10, -1, -1);
        slider_brightness = new QSlider(GeneralSettingsPage);
        slider_brightness->setObjectName(QStringLiteral("slider_brightness"));
        slider_brightness->setOrientation(Qt::Horizontal);

        horizontalLayout_9->addWidget(slider_brightness);


        verticalLayout->addLayout(horizontalLayout_9);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(GeneralSettingsPage);

        QMetaObject::connectSlotsByName(GeneralSettingsPage);
    } // setupUi

    void retranslateUi(QWidget *GeneralSettingsPage)
    {
        GeneralSettingsPage->setWindowTitle(QApplication::translate("GeneralSettingsPage", "GeneralSettingsPage", nullptr));
        label->setText(QApplication::translate("GeneralSettingsPage", "When the power button is pressed", nullptr));
        label_2->setText(QApplication::translate("GeneralSettingsPage", "When the suspend button is pressed", nullptr));
        label_3->setText(QApplication::translate("GeneralSettingsPage", "When closing the lid", nullptr));
        label_brightness->setText(QApplication::translate("GeneralSettingsPage", "Display brightness setting", nullptr));
        label_brightnessPercent->setText(QApplication::translate("GeneralSettingsPage", "0%", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GeneralSettingsPage: public Ui_GeneralSettingsPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GENERAL_2D_SETTINGS_2D_PAGE_H
