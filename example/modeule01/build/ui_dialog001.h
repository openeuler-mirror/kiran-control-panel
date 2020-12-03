/********************************************************************************
** Form generated from reading UI file 'dialog001.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG001_H
#define UI_DIALOG001_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Dialog001
{
public:
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton;
    QLabel *label;
    QLineEdit *lineEdit;

    void setupUi(QDialog *Dialog001)
    {
        if (Dialog001->objectName().isEmpty())
            Dialog001->setObjectName(QLatin1String("Dialog001"));
        Dialog001->resize(400, 300);
        verticalLayout = new QVBoxLayout(Dialog001);
        verticalLayout->setObjectName(QLatin1String("verticalLayout"));
        pushButton = new QPushButton(Dialog001);
        pushButton->setObjectName(QLatin1String("pushButton"));

        verticalLayout->addWidget(pushButton);

        label = new QLabel(Dialog001);
        label->setObjectName(QLatin1String("label"));

        verticalLayout->addWidget(label);

        lineEdit = new QLineEdit(Dialog001);
        lineEdit->setObjectName(QLatin1String("lineEdit"));

        verticalLayout->addWidget(lineEdit);


        retranslateUi(Dialog001);

        QMetaObject::connectSlotsByName(Dialog001);
    } // setupUi

    void retranslateUi(QDialog *Dialog001)
    {
        Dialog001->setWindowTitle(QApplication::translate("Dialog001", "Dialog", nullptr));
        pushButton->setText(QApplication::translate("Dialog001", "3.0", nullptr));
        label->setText(QApplication::translate("Dialog001", "Dialog001", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog001: public Ui_Dialog001 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG001_H
