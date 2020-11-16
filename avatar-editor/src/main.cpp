#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QPixmap>
#include <QFileInfo>
#include <QDir>
#include <QTranslator>

#include "tools/log.h"
#include "include/exit-code-defines.h"
#include "kiran-avatar-editor.h"

//预览图像路径
QString prewview_image;
//裁剪过后的的图片保存路径
QString cliped_image_save_path;

void handlerCommandOption(const QApplication &app)
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

    cmdParser.addOptions({previewImageOption,clipedImageSavePathOption});
    cmdParser.addHelpOption();
    cmdParser.process(app);

    QString tempPath;
    if( !cmdParser.isSet( previewImageOption ) ){
        qWarning() << "missing  parameter(--image)";
        exit(EXIT_CODE_MISSING_PARAMTER);
    }else{
        tempPath = cmdParser.value(previewImageOption);
        QPixmap pixmap;
        if( !pixmap.load(tempPath) ){
            qWarning() << "preview image (" << tempPath << ") is invalid";
            exit(EXIT_CODE_BAD_ARG);
        }else{
            prewview_image = tempPath;
        }
    }

    if( !cmdParser.isSet(clipedImageSavePathOption) ){
        qWarning() << "missing parameter(--cliped-save-path)";
        exit(EXIT_CODE_MISSING_PARAMTER);
    }else{
        tempPath = cmdParser.value(clipedImageSavePathOption);
        QFileInfo fileInfo(tempPath);
        QDir dir = fileInfo.dir();
        QFileInfo dirInfo(dir.absolutePath());

        if( !dir.exists() && !dir.mkpath(dir.absolutePath()) ){
            qWarning() << QString("craete dir(%1) failed.").arg(dir.absolutePath());
            exit(EXIT_CODE_BAD_ARG);
        }
        if( !dirInfo.isWritable() ){
            qWarning() << QString("dir(%1) can't write.").arg(dir.absolutePath());
            exit(EXIT_CODE_BAD_ARG);
        }
        if( fileInfo.exists() ){
            qWarning() << QString("cliped image save path(%1) is exist.").arg(tempPath);
            exit(EXIT_CODE_BAD_ARG);
        }
        cliped_image_save_path = tempPath;
    }
    qInfo() << "preview image:" << prewview_image;
    qInfo() << "cliped image save path:" << cliped_image_save_path;
}

void loadStylesheet(){
    QFile file(":/themes/avatar-editor_back.qss");
    if(file.open(QIODevice::ReadOnly)){
        QString style =  file.readAll();
        qApp->setStyleSheet(style);
    }else{
        qWarning() << "load stylesheet failed.";
     }
}

int main(int argc,char* argv[])
{
    QApplication app(argc,argv);
    app.setQuitOnLastWindowClosed(false);

    Log::instance()->init("/tmp/kiran-account-manager.log");
    qInstallMessageHandler(Log::messageHandler);

    QString translationDir = QString("/usr/share/%1/translations/").arg(qAppName());
    QTranslator tsor;
    tsor.load(QLocale(),
              qAppName()/*filename*/,
              "."/*prefix*/,
              translationDir,
              ".qm"/*suffix*/);
    app.installTranslator(&tsor);

    handlerCommandOption(app);

    loadStylesheet();

    KiranAvatarEditor avatarEditor(prewview_image,cliped_image_save_path);
    avatarEditor.show();

    return QApplication::exec();
}
