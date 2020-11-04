//
// Created by lxh on 2020/10/27.
//

#include "avatar-editor-wrapper.h"
#include "include/global-defines.h"
#include "temporary-dir-manager.h"
#include "avatar-editor/include/exit-code-defines.h"

#include <QProcess>
#include <QDebug>
#include <QMap>

QString avatarEditorError(int exitCode){
    static const QMap<int,QString> exitCodeMap = {
            {0,"success"},
            {1,"user cancel"},
            {2,"bad arg"},
            {3,"save failed"},
            {4,"missing paramter"}
    };
    static QString unknowError = "unknow exit code";

    auto iter = exitCodeMap.find(exitCode);
    if(iter==exitCodeMap.end()){
        return unknowError;
    }else{
        return iter.value();
    }
}

bool AvatarEditorWrapper::exec(const QString &srcImage,QString &dstImage) {
    QProcess avatarEditorProcess;
    QString tempFilePath = TemporaryDirManager::instance()->generateTempFilePath();

    avatarEditorProcess.start(KIRAN_AVATAR_EDITOR_PATH,
                              QStringList() << "--image" << srcImage
                                            << "--cliped-save-path" << tempFilePath,
                                            QIODevice::NotOpen);

    if( !avatarEditorProcess.waitForStarted(3000) ){
        return false;
    }

    avatarEditorProcess.waitForFinished();

    if( avatarEditorProcess.exitCode() == EXIT_CODE_SUCCESS ){
        dstImage = tempFilePath;
        return true;
    }

    return false;
}