/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef DIALOG001_H
#define DIALOG001_H

#include "plugin-subitem-base.h"
#include <QDialog>

namespace Ui {
class Dialog001;
}

class Dialog001 : public QDialog, public PluginSubItemBase
{
    Q_OBJECT

public:
    explicit Dialog001(QWidget *parent = 0);
    ~Dialog001();
    bool hasUnsavedOptions(){return gHasUnsavedOptions;}

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

    void on_pushButton_clicked();

private:
    Ui::Dialog001 *ui;
    bool gHasUnsavedOptions;
};

#endif // DIALOG001_H
