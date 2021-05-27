/***************************************************************************
 *                                                                         *
 *   Copyright ©2020 KylinSec. All rights reserved.                      *
 *                                                                         *
 ***************************************************************************/
#ifndef DIALOG002_H
#define DIALOG002_H

#include <QDialog>

namespace Ui {
class SubItem2;
}

class SubItem2 : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(bool unsaved READ unsaved)
public:
    explicit SubItem2(QWidget *parent = 0);
    ~SubItem2();

    bool unsaved();
private slots:
    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::SubItem2 *ui;
    bool m_unsaved = false;
};

#endif // DIALOG002_H
