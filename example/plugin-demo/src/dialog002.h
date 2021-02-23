/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef DIALOG002_H
#define DIALOG002_H

#include "plugin-subitem-base.h"
#include <QDialog>

namespace Ui {
class Dialog002;
}

class Dialog002 : public QDialog, public PluginSubItemBase
{
    Q_OBJECT

public:
    explicit Dialog002(QWidget *parent = 0);
    ~Dialog002();
    bool hasUnsavedOptions(){return gHasUnsavedOptions;}

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::Dialog002 *ui;
    bool gHasUnsavedOptions;
};

#endif // DIALOG002_H
