/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#ifndef USERINFOPAGE_H
#define USERINFOPAGE_H

#include "kiran-tips/kiran-tips.h"

#include <QWidget>

namespace Ui
{
class UserInfoPage;
}
class HoverTips;
class KiranSwitchButton;
class UserInfoPage : public QWidget
{
    Q_OBJECT
    friend class AccountWidget;

public:
    explicit UserInfoPage(QWidget *parent = nullptr);
    ~UserInfoPage();

    bool eventFilter(QObject *watched, QEvent *event) override;

    /// 　从Accounts服务中重新加载用户信息
    void updateInfo();

    /// 设置当前显示的用户的DBus对象路径
    /// \param userObj 用户DBus对象路径
    void setCurrentShowUserPath(const QString &userObj);

    /// 获取当前显示的用户DBus路径
    /// \return 当前显示用户DBus路径
    QString getCurrentShowUserPath();

    /// 获取当前显示的用户名
    /// \return 用户名
    QString getCurrentShowUserName();

    /// 外部设置头像路径
    /// \param iconPath 头像路径
    void setAvatarIconPath(const QString &iconPath);

signals:
    /// 当用户点击头像时，请求切换到头像选择界面，参数为
    /// \param iconPath 当前所选头像的路径
    void requestIconPage(const QString &iconPath);

    /// @brief 当用户点击密码过期策略时，跳转到密码过期策略界面
    /// @param userObj 密码过期策略的DBus对象
    void requestPasswordExpirationPolicy(const QString &userObj);

    /// 当用户点击保存按钮时,发出的保存用户属性信号，请求工作线程处理
    /// \param userName     用户名
    /// \param iconFile     头像路径
    /// \param userType     用户类型
    /// \param isLocked     是否锁定
    void requestUpdateUserProperty(QString objPath,
                                   QString userName,
                                   QString iconFile,
                                   int userType,
                                   bool isLocked);

    /// 当用户点击保存新密码时,发出的更新密码信号，请求工作线程处理
    /// \param userName         用户名
    /// \param encryptedPasswd  加密过的密码
    void requestUpdatePasswd(QString objPath,
                             QString userName,
                             QString encryptedCurPasswd,
                             QString encryptedPasswd);

    /// 当用户点击删除用户时发出的删除用户信号，请求工作线程处理
    /// \param uid  删除用户的UID
    void requestDeleteUser(int uid);

    /// 当前忙碌状态改变的信号，显示/隐藏遮罩
    /// \param isBusy 是否忙碌
    void busyChanged(bool isBusy);

private:
    /// 初始化界面
    void initUI();

    /// 重置用户信息页面中的设置密码二级页面
    void resetPageSetPasswd();

private Q_SLOTS:
    /// 处理用户保存属性的槽函数
    void handlerUpdateUserProperty();
    /// 用户保存属性完成的槽函数回调
    void handlerUpdateUserPropertyDone(QString errMsg);

    void handlerDeleteUser();
    void handlerDeleteUserDone(QString errMsg);

    /// 　处理用户保存密码的槽函数
    void handlerUpdatePasswd();
    /// 用户保存密码完成的槽函数回调
    void handlerUpdatePasswdDone(QString errMsg);

private:
    Ui::UserInfoPage *ui;
    QString m_curShowUserPath;
    QString m_curShowUserName;
    uid_t m_uid;
    KiranTips *m_errorTip;
    HoverTips *m_hoverTip;
    KiranSwitchButton *m_userStatusSwitch;
};

#endif  // USERINFOPAGE_H
