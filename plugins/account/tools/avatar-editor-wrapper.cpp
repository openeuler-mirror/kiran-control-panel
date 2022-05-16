/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd. 
 * kiran-cpanel-account is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2. 
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2 
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, 
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.  
 * See the Mulan PSL v2 for more details.  
 * 
 * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
 */

#include "avatar-editor-wrapper.h"
#include "exit-code-defines.h"
#include "kcp-account-config.h"
#include "global-defines.h"
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
        KLOG_WARNING() << "can't start" << KIRAN_AVATAR_EDITOR_PATH;
        return false;
    }

    avatarEditorProcess.waitForFinished();

    if (avatarEditorProcess.exitCode() == EXIT_CODE_SUCCESS)
    {
        dstImage = tempFilePath;
        return true;
    }
    KLOG_WARNING() << "kiran-avatar-editor:" << avatarEditorError(avatarEditorProcess.exitCode());
    return false;
}