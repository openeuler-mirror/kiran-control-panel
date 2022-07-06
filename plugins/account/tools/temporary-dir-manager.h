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

#ifndef KIRAN_AVATAR_EDITOR_TEMPORARY_DIR_MANAGER_H
#define KIRAN_AVATAR_EDITOR_TEMPORARY_DIR_MANAGER_H

#include <QString>

class TemporaryDirManager
{
public:
    static TemporaryDirManager *instance();
    ~TemporaryDirManager();

    //初始化,创建临时目录(若存在则删除)
    bool init(const QString &dirName);

    //生成临时目录下唯一文件名
    QString generateTempFilePath();

private:
    TemporaryDirManager();
    //删除临时目录
    void remove();

private:
    bool m_initFinished = false;
    QString m_temporaryDirPath;
};

#endif  //KIRAN_AVATAR_EDITOR_TEMPORARY_DIR_MANAGER_H
