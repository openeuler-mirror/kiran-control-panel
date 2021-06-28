//
// Created by lxh on 2020/11/10.
//

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
