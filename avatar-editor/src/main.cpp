#include <kiran-application.h>
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QPixmap>
#include <QTranslator>
#include <qt5-log-i.h>
#include <iostream>

#include "../config.h"
#include "include/exit-code-defines.h"
#include "kiran-avatar-editor.h"

//预览图像路径
QString prewview_image;
//裁剪过后的的图片保存路径
QString cliped_image_save_path;

void handlerCommandOption(const QApplication& app)
{
    QCommandLineParser cmdParser;
    QCommandLineOption previewImageOption("image",
                                          "preview image",
                                          "image path",
                                          "");
    QCommandLineOption clipedImageSavePathOption("cliped-save-path",
                                                 "cliped image save path",
                                                 "save path",
                                                 "");

    cmdParser.addOptions({previewImageOption, clipedImageSavePathOption});
    cmdParser.addHelpOption();
    cmdParser.process(app);

    QString tempPath;
    if (!cmdParser.isSet(previewImageOption))
    {
        std::cerr << "missing  parameter(--image)" << std::endl;
        exit(EXIT_CODE_MISSING_PARAMTER);
    }
    else
    {
        tempPath = cmdParser.value(previewImageOption);
        QPixmap pixmap;
        if (!pixmap.load(tempPath))
        {
            std::cerr << "preview image (" << tempPath.toStdString() << ") is invalid" << std::endl;
            exit(EXIT_CODE_BAD_ARG);
        }
        else
        {
            prewview_image = tempPath;
        }
    }

    if (!cmdParser.isSet(clipedImageSavePathOption))
    {
        std::cerr << "missing parameter(--cliped-save-path)" << std::endl;
        exit(EXIT_CODE_MISSING_PARAMTER);
    }
    else
    {
        tempPath = cmdParser.value(clipedImageSavePathOption);
        QFileInfo fileInfo(tempPath);
        QDir dir = fileInfo.dir();
        QFileInfo dirInfo(dir.absolutePath());

        if (!dir.exists() && !dir.mkpath(dir.absolutePath()))
        {
            std::cerr << QString("craete dir(%1) failed.").arg(dir.absolutePath()).toStdString() << std::endl;
            exit(EXIT_CODE_BAD_ARG);
        }
        if (!dirInfo.isWritable())
        {
            std::cerr << QString("dir(%1) can't write.").arg(dir.absolutePath()).toStdString() << std::endl;
            exit(EXIT_CODE_BAD_ARG);
        }
        if (fileInfo.exists())
        {
            std::cerr << QString("cliped image save path(%1) is exist.").arg(tempPath).toStdString() << std::endl;
            exit(EXIT_CODE_BAD_ARG);
        }
        cliped_image_save_path = tempPath;
    }

    KLOG_INFO_S() << "preview image:" << prewview_image;
    KLOG_INFO_S() << "cliped image save path:" << cliped_image_save_path;
}

void loadStylesheet()
{
    QFile file(":/kcp-account-themes/avatar-editor_back.qss");
    if (file.open(QIODevice::ReadOnly))
    {
        QString style = file.readAll();
        qApp->setStyleSheet(style);
    }
    else
    {
        KLOG_WARNING_S() << "load stylesheet failed.";
    }
}

int main(int argc, char* argv[])
{
    KiranApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);


    klog_qt5_init("","kylinsec-session","kiran-cpanel-account","avatar-editor");

    QTranslator tsor;
    tsor.load(QLocale(),
              qAppName() /*filename*/,
              "." /*prefix*/,
              TRANSLATION_DIR_PATH,
              ".qm" /*suffix*/);
    QApplication::installTranslator(&tsor);

    handlerCommandOption(app);

    loadStylesheet();

    KiranAvatarEditor avatarEditor(prewview_image, cliped_image_save_path);
    avatarEditor.show();

    return QApplication::exec();
}
