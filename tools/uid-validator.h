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
 
#ifndef UIDVALIDATOR_H
#define UIDVALIDATOR_H

#include <QValidator>

class UidValidator : public QValidator
{
    Q_OBJECT
public:
    explicit UidValidator(QObject *parent = nullptr);

    virtual State validate(QString &, int &) const Q_DECL_OVERRIDE;
};

#endif  // UIDVALIDATOR_H
