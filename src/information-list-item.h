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

#ifndef INFORMATIONLISTITEM_H
#define INFORMATIONLISTITEM_H

#include <QWidget>

namespace Ui {
class InformationListItem;
}

class InformationListItem : public QWidget
{
    Q_OBJECT

public:
    explicit InformationListItem(QWidget *parent = 0);
    ~InformationListItem();

protected:
    void paintEvent(QPaintEvent*event);

public:
    void setItemText(const QString &text);
    void setItemIcon(const QString &iconPath);
    void setItemArrow(bool isSelected);
    virtual QSize sizeHint() const override;

private:
    Ui::InformationListItem *ui;
};

#endif // InformationListItem_H
