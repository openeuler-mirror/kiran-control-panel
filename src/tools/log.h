/**
 * @file general-functions-class.h
 * @brief 处理打印的日志信息
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#ifndef LOG_H
#define LOG_H

#include <QMessageLogContext>
#include <QString>
#include <QSize>

class Log
{
public:
    Log();

public:
    static void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString& msg);
};

#endif // LOGS_H
