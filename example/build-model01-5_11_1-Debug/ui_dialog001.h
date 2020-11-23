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
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Dialog001
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Dialog001)
    {
        if (Dialog001->objectName().isEmpty())
            Dialog001->setObjectName(QLatin1String("Dialog001"));
        Dialog001->resize(400, 300);
        verticalLayout = new QVBoxLayout(Dialog001);
        verticalLayout->setObjectName(QLatin1String("verticalLayout"));
        label = new QLabel(Dialog001);
        label->setObjectName(QLatin1String("label"));

        verticalLayout->addWidget(label);

        lineEdit = new QLineEdit(Dialog001);
        lineEdit->setObjectName(QLatin1String("lineEdit"));

        verticalLayout->addWidget(lineEdit);

        buttonBox = new QDialogButtonBox(Dialog001);
        buttonBox->setObjectName(QLatin1String("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(Dialog001);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog001, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog001, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog001);
    } // setupUi

    void retranslateUi(QDialog *Dialog001)
    {
        Dialog001->setWindowTitle(QApplication::translate("Dialog001", "Dialog", nullptr));
        label->setText(QApplication::translate("Dialog001", "Dialog001", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog001: public Ui_Dialog001 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG001_H
