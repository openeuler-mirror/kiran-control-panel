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
