/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

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
