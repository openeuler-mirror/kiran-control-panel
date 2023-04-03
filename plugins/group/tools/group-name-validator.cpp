/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-cpanel-group is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     wangshichang <shichang@isrc.iscas.ac.cn>
 */
#include "group-name-validator.h"

#include <QDebug>

GroupNameValidator::GroupNameValidator(QObject *parent) : QValidator(parent)
{
}

/// TODO:未定义规则
QValidator::State GroupNameValidator::validate(QString &input, int &pos) const
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
