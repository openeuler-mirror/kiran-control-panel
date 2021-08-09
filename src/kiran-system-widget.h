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

#ifndef KIRANSYSTEMWIDGET_H
#define KIRANSYSTEMWIDGET_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include "kiran-system-information.h"

#include <QPushButton>
#include <QMenu>

class KiranSystemWidget : public KiranTitlebarWindow
{
    Q_OBJECT
public:
    explicit KiranSystemWidget();
    ~KiranSystemWidget();
    virtual QSize sizeHint() const override;

private:
     kiranSystemInformation *centerWgt;

};

#endif // KIRANSYSTEMWIDGET_H
