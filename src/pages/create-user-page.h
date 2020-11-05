#ifndef CREATEUSERPAGE_H
#define CREATEUSERPAGE_H

#include <QWidget>
#include <QThread>

#include "advance-settings.h"

class KiranTips;

namespace Ui {
class CreateUserPage;
}

class CreateUserPage : public QWidget
{
    friend class KiranAccountManager;
    Q_OBJECT
public:
    explicit CreateUserPage(QWidget *parent = nullptr);
    ~CreateUserPage();

    /// 重置界面信息，每次切换到创建用户界面时需重置信息
    void reset();

    /// 设置创建用户界面用户图标，在从用户头像选择界面中退出时设置更新
    /// \param 用户选择的头像路径
    void setAvatarIconPath(const QString &iconPath);

signals:
    /// 当用户点击创建用户按钮时,经过基础检查发出的创建用户信号，请求工作线程处理
    /// \param account          账户名
    /// \param uid              用户id
    /// \param accountType      账户类型
    /// \param encryptedPasswd  密码加密
    /// \param homeDir          家目录
    /// \param shell            shell路径
    /// \param iconFile         图标路路径
    void sigCreateUser(const QString &account,
                       int uid,
                       int accountType,
                       const QString &encryptedPasswd,
                       const QString &homeDir,
                       const QString &shell,
                       const QString &iconFile);

    /// 当用户点击头像时，请求切换到头像选择界面，参数为
    /// \param 当前所选头像的路径
    void sigSetIconForNewUser(QString iconPath);

    /// 创建用户完成时，请求设置侧边栏当前条目,跳转到新用户信息页面
    /// \param userPath 新用户的DBus对象路径
    void sigRequestSetCurrentUser(const QString& userPath);

    /// 当前忙碌状态改变的信号，显示/隐藏遮罩
    /// \param isBusy 是否忙碌
    void sigIsBusyChanged(bool isBusy);

private:
    ///　初始化界面
    void initUI();

private Q_SLOTS:
    ///　创建用户槽函数
    void handlerCreateNewUser();

    /// 工作线程创建完用户的槽函数回调
    /// \param userPath 新用户的DBus对象路径
    /// \param errMsg   错误信息
    void handlerCreateNewUserIsDone(const QString &userPath,const QString &errMsg);

private:
    Ui::CreateUserPage *ui;
    /// 高级设置中的信息
    AdvanceSettingsInfo m_advanceSettingsInfo;
    /// 错误悬浮提示框
    KiranTips *m_errorTip;
};

#endif // CREATEUSERPAGE_H
