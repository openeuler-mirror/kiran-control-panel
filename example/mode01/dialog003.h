/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef DIALOG003_H
#define DIALOG003_H

#include "modelbase.h"
#include <QDialog>

namespace Ui {
class Dialog003;
}

class Dialog003 : public QDialog, public modelbase
{
    Q_OBJECT

public:
    explicit Dialog003(QWidget *parent = 0);
    ~Dialog003();

private:
    Ui::Dialog003 *ui;
};

#endif // DIALOG003_H
