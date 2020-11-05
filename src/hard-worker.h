//
// Created by lxh on 2020/11/5.
//

#ifndef KIRAN_ACCOUNT_MANAGER_HARD_WORKER_H
#define KIRAN_ACCOUNT_MANAGER_HARD_WORKER_H

#include <QObject>

class HardWorker :public QObject{
    Q_OBJECT
public:
    explicit HardWorker();
    ~HardWorker();

public slots:
    //创建用户
    void doCreateUser(const QString &account,
                      int uid,
                      int accountType,
                      const QString &encryptedPasswd,
                      const QString &homeDir,
                      const QString &shell,
                      const QString &iconFile);

signals:
    /// 创建用户完成信号
    /// \param userPath 创建完成的用户DBus对象路径,若创建用户都失败，则为空
    /// \param errMsg   错误消息框，收到弹出提示框
    void createUserisDnoe(const QString &userPath,const QString &errMsg);

};


#endif //KIRAN_ACCOUNT_MANAGER_HARD_WORKER_H
