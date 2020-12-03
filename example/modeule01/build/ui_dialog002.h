/********************************************************************************
** Form generated from reading UI file 'dialog002.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG002_H
#define UI_DIALOG002_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Dialog002
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *lineEdit;

    void setupUi(QDialog *Dialog002)
    {
        if (Dialog002->objectName().isEmpty())
            Dialog002->setObjectName(QLatin1String("Dialog002"));
        Dialog002->resize(400, 300);
        verticalLayout = new QVBoxLayout(Dialog002);
        verticalLayout->setObjectName(QLatin1String("verticalLayout"));
        label = new QLabel(Dialog002);
        label->setObjectName(QLatin1String("label"));

        verticalLayout->addWidget(label);

        lineEdit = new QLineEdit(Dialog002);
        lineEdit->setObjectName(QLatin1String("lineEdit"));

        verticalLayout->addWidget(lineEdit);


        retranslateUi(Dialog002);

        QMetaObject::connectSlotsByName(Dialog002);
    } // setupUi

    void retranslateUi(QDialog *Dialog002)
    {
        Dialog002->setWindowTitle(QApplication::translate("Dialog002", "Dialog", nullptr));
        label->setText(QApplication::translate("Dialog002", "Dialog002", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog002: public Ui_Dialog002 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG002_H
