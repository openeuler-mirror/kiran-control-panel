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
