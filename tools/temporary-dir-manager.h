//
// Created by lxh on 2020/10/26.
//

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
