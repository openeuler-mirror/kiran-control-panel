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
 
#include "uid-validator.h"

#include <QDebug>

UidValidator::UidValidator(QObject *parent)
    : QValidator(parent)
{
}

QValidator::State UidValidator::validate(QString &input, int &pos) const
{
    ///为空
    if (pos == 0 || input.isEmpty())
    {
        return Acceptable;
    }
    ///第一个字符为0
    if (pos == 1 && input == "0")
    {
        return Invalid;
    }
    ///输入的字符不为数字
    QChar ch = input.at(pos - 1);
    if (!ch.isNumber())
    {
        return Invalid;
    }
    ///数字大于UID_MAX
    if (input.toULongLong() >= ((uid_t)-1))
    {
        return Invalid;
    }
    return Acceptable;
}
