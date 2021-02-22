/**
 * @file general-functions-class.h
 * @brief 通用的一些函数
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#ifndef GENERALFUNCTIONSCLASS_H
#define GENERALFUNCTIONSCLASS_H

#include <QMessageLogContext>
#include <QString>
#include <QSize>

class GeneralFunctionsClass
{
public:
    GeneralFunctionsClass();

public:
    static void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString& msg);
};

#endif // GENERALFUNCTIONSCLASS_H
