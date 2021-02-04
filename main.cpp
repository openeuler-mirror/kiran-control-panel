#include "kiran-system-information.h"
#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QLoggingCategory>

#include "general-functions-class.h"
#include "single/singleapplication.h"

int main(int argc, char *argv[])
{
    ///注册自定义的消息处理函数
    qInstallMessageHandler(GeneralFunctionsClass::customMessageHandler);
    QLoggingCategory::defaultCategory()->setEnabled(QtMsgType::QtDebugMsg,true);
    qDebug("******New Output*********\n");

    SingleApplication a(argc, argv);

    ///加载qss样式表
    QFile file(":/qss/data/style.qss");
    if( file.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(file.readAll());

        a.setStyleSheet(styleSheet);
        file.close();
    }
    else
    {
        QMessageBox::warning(NULL, "warning", "Open failed", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }

    ///加载翻译文件

    kiranSystemInformation w;
    w.show();

    return a.exec();
}
