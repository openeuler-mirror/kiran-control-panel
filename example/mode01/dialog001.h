/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef DIALOG001_H
#define DIALOG001_H

#include "modelbase.h"
#include <QDialog>

namespace Ui {
class Dialog001;
}

class Dialog001 : public QDialog, public modelbase
{
    Q_OBJECT

public:
    explicit Dialog001(QWidget *parent = 0);
    ~Dialog001();

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::Dialog001 *ui;
};

#endif // DIALOG001_H
