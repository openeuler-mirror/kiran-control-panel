/**
 * @file general-functions-class.cpp
 * @brief 通用的一些函数
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#include "general-functions-class.h"
#include <QFile>
#include <QMessageBox>
#include <QtMessageHandler>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMutex>
#include <QDateTime>
#include <iostream>
#include <zlog_ex.h>
#include <stddef.h>

using namespace std;
#define EMPTY_NAME "null"
extern bool init_zlog;

GeneralFunctionsClass::GeneralFunctionsClass()
{

}

/**
 * @brief GeneralFunctionsClass::customMessageHandler:自定义调试信息
 * @param type 消息类型
 * @param context 消息输出位置
 * @param msg 消息内容
 */
void GeneralFunctionsClass::customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();
    QString text;
    int32_t priority;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug...............................");
        priority = ZLOG_LEVEL_DEBUG;
        break;
    case QtInfoMsg:
        text = QString("Infomation...............................");
        priority = ZLOG_LEVEL_INFO;
        break;
    case QtWarningMsg:
        text = QString("Warning...............................");
        priority = ZLOG_LEVEL_WARN;
        break;
    case QtCriticalMsg:
        text = QString("Critical...............................");
        priority = ZLOG_LEVEL_ERROR;
        break;
    case QtFatalMsg:
        text = QString("Fatal...............................");
        priority = ZLOG_LEVEL_FATAL;
        abort();
    }

    const char* file_name = context.file == NULL?EMPTY_NAME:context.file;
    const char* function_name =  context.function == NULL?EMPTY_NAME:context.function;
    if (init_zlog){
    dzlog(file_name,
          strlen(file_name),
          function_name,
          strlen(function_name),
          context.line,
          priority,
          "%s",
          msg.toStdString().c_str());
    }
    mutex.unlock();
}
