#ifndef GENERALFUNCTIONSCLASS_H
#define GENERALFUNCTIONSCLASS_H

#include <QMessageLogContext>
#include <QString>
class GeneralFunctionsClass
{
public:
    GeneralFunctionsClass();

public:
    static void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString& msg);
};

#endif // GENERALFUNCTIONSCLASS_H
