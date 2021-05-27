//
// Created by lxh on 2020/10/27.
//

#include "avatar-editor-wrapper.h"
#include "avatar-editor/include/exit-code-defines.h"
#include "config.h"
#include "include/global-defines.h"
#include "temporary-dir-manager.h"

#include <QDebug>
#include <QMap>
#include <QProcess>
#include <qt5-log-i.h>

QString avatarEditorError(int exitCode)
{
    static const QMap<int, QString> exitCodeMap = {
        {EXIT_CODE_SUCCESS, "success"},
        {EXIT_CODE_CANCEL, "user cancel"},
        {EXIT_CODE_BAD_ARG, "bad arg"},
        {EXIT_CODE_SAVE_FAILED, "save failed"},
        {EXIT_CODE_MISSING_PARAMTER, "missing paramter"}};
    static QString unknowError = "unknow exit code";

    auto iter = exitCodeMap.find(exitCode);
    if (iter == exitCodeMap.end())
    {
        return unknowError;
    }
    else
    {
        return iter.value();
    }
}

bool AvatarEditorWrapper::exec(const QString &srcImage, QString &dstImage)
{
    QProcess avatarEditorProcess;
    QString tempFilePath = TemporaryDirManager::instance()->generateTempFilePath();

    avatarEditorProcess.start(KIRAN_AVATAR_EDITOR_PATH,
                              QStringList() << "--image" << srcImage
                                            << "--cliped-save-path" << tempFilePath,
                              QIODevice::NotOpen);

    if (!avatarEditorProcess.waitForStarted(3000))
    {
        KLOG_WARNING_S() << "can't start" << KIRAN_AVATAR_EDITOR_PATH;
        return false;
    }

    avatarEditorProcess.waitForFinished();

    if (avatarEditorProcess.exitCode() == EXIT_CODE_SUCCESS)
    {
        dstImage = tempFilePath;
        return true;
    }
    KLOG_WARNING_S() << "kiran-avatar-editor:" << avatarEditorError(avatarEditorProcess.exitCode());
    return false;
}