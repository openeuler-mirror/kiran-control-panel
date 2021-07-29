 /**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
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
 
#ifndef KIRAN_ACCOUNT_MANAGER_MASK_WIDGET_H
#define KIRAN_ACCOUNT_MANAGER_MASK_WIDGET_H

#include <QWidget>

class MaskWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MaskWidget(QWidget *parent);
    ~MaskWidget() = default;

private Q_SLOTS:

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QWidget *const m_maskedWidget;
};

#endif  //KIRAN_ACCOUNT_MANAGER_MASK_WIDGET_H
