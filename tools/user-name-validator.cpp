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
 

#include "user-name-validator.h"

#include <QDebug>

UserNameValidator::UserNameValidator(QObject *parent) : QValidator(parent)
{
}

/// 只允许输入数字、字母、下划线
QValidator::State UserNameValidator::validate(QString &input, int &pos) const
{
    ///为空
    if (pos == 0 || input.isEmpty())
    {
        return Acceptable;
    }

    QChar ch = input.at(pos - 1);
    if (!ch.isNumber() &&
        !ch.isUpper() &&
        !ch.isLower() &&
        (ch != QChar('_')))
    {
        return Invalid;
    }

    return QValidator::Acceptable;
}
