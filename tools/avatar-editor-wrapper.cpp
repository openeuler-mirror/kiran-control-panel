 /**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd. 
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; If not, see <http: //www.gnu.org/licenses/>. 
  */
 
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