#include "uid-validator.h"

#include <QDebug>


UidValidator::UidValidator(QObject *parent)
    :QValidator(parent)
{

}

QValidator::State UidValidator::validate(QString &input, int &pos) const
{
    ///为空
    if(pos==0||input.isEmpty()){
        return Acceptable;
    }
    ///第一个字符为0
    if( pos==1 && input=="0" ){
        return Invalid;
    }
    ///输入的字符不为数字
    QChar ch = input.at(pos-1);
    if( !ch.isNumber() ){
        return Invalid;
    }
    ///数字大于UID_MAX
    if( input.toULongLong() >= ((uid_t)-1)){
        return Invalid;
    }
    return Acceptable;
}

