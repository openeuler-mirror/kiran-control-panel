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

using namespace std;
#define OUTPUTFILE "/tmp/kiran-system-information.log"

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
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug...............................");
        break;
    case QtInfoMsg:
        text = QString("Infomation...............................");
        break;
    case QtWarningMsg:
        text = QString("Warning...............................");
        break;
    case QtCriticalMsg:
        text = QString("Critical...............................");
        break;
    case QtFatalMsg:
        text = QString("Fatal...............................");
        abort();
    }
    //消息输出位置
    QString context_info = QString("File: %1\r\nFunc: %2\r\nLine: %3")
            .arg(QString(context.file))
            .arg(QString(context.function))
            .arg(context.line);
    //消息打印时间
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("Time: %1")
            .arg(current_date_time);
    //调试信息
    QString mmsg = QString("%1\r\n%2\r\n%3\r\n%4")
            .arg(text)
            .arg(current_date)
            .arg(context_info)
            .arg("MSG : "+msg);


    if(!qgetenv("OutputInfo").isNull())
    {
        if(qgetenv("OutputInfo")== "0")
        {
            char*  output;
            QByteArray ba = mmsg.toLatin1(); // must
            output=ba.data();
            std::cout <<output <<std::endl;
            mutex.unlock();
            return ;
        }
    }
    QFile file(OUTPUTFILE);
    QFileInfo fileInfo(OUTPUTFILE);
    //调试文件没有被创建过
    if(!fileInfo.dir().exists() && !fileInfo.dir().mkpath(fileInfo.dir().path()))
    {
       fprintf(stderr,"make log file failed\n");
       goto Create;
    }

Create:
    while(!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {

        cout << "open log file " << endl;
        QFlags<QFile::Permission> flags = QFile::ReadOwner|QFile::WriteOwner|
                                                          QFile::ReadUser |QFile::WriteUser |
                                                          QFile::ReadGroup|QFile::WriteGroup|
                                                          QFile::ReadOther|QFile::WriteOther;
//        file.setPermissions(QFile::ReadOwner|QFile::WriteOwner|
//                            QFile::ReadUser |QFile::WriteUser |
//                            QFile::ReadGroup|QFile::WriteGroup|
//                            QFile::ReadOther|QFile::WriteOther);
        if( file.permissions() != flags ){
            if(file.setPermissions(flags))
            {
                cout << "set permission ok" << endl;
            }
            else
            {
                cout << "set permission failed " << endl;
            }
        }
        else
        {
            cout << "permission  == flag " << endl;
        }
    }
    QTextStream stream(&file);
    stream << mmsg << "\r\n";
    file.flush();
    file.close();
    mutex.unlock();

}
