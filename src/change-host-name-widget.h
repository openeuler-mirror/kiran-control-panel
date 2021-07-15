/**
 * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http: //www.gnu.org/licenses/>.
 */

#ifndef CHANGEHOSTNAMEWIDGET_H
#define CHANGEHOSTNAMEWIDGET_H

#include <QWidget>
#include <kiranwidgets-qt5/kiran-titlebar-window.h>

namespace Ui {
class ChangeHostNameWidget;
}

class ChangeHostNameWidget : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit ChangeHostNameWidget();
    ~ChangeHostNameWidget();
    bool getLineEditStatus();

private:
    void initUI();

signals:
    void sigChangeNameSuccessful(bool isChanged, QString name);

private slots:
    void setNewHostName();
    void setInputEditStatus();

private:
    Ui::ChangeHostNameWidget *ui;
};

#endif // CHANGEHOSTNAMEWIDGET_H
