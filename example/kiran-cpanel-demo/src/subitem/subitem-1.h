/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef DIALOG001_H
#define DIALOG001_H

#include <QDialog>

namespace Ui {
class SubItem1;
}

class SubItem1 : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(bool unsaved READ unsaved)
public:
    explicit SubItem1(QWidget *parent = 0);
    ~SubItem1();

    bool unsaved();

private slots:
    void on_lineEdit_textChanged(const QString &arg1);
    void on_pushButton_clicked();

private:
    Ui::SubItem1 *ui;
    bool m_unsaved = false;
};

#endif // DIALOG001_H
