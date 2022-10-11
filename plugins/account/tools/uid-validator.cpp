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
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
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
