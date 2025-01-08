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

#ifndef SELECTAVATARPAGE_H
#define SELECTAVATARPAGE_H

#include <QWidget>

class QLayout;
class FlowLayout;
class QButtonGroup;
class QScrollArea;
class AvatarButton;

class SelectAvatarPage : public QWidget
{
    Q_OBJECT
public:
    ///@biref 描述是由什么页面跳转而来
    ///       在用户点击确定或取消之后，发送sigReturnToPrevPage携带该参数
    enum SelectAvatarMode
    {
        SELECT_AVATAR_FOR_NEW_USER,
        CHANGE_AVATAR_FOR_USER
    };
    Q_ENUM(SelectAvatarMode);

    enum AvatarTypeEnum
    {
        AVATAR_SYSTEM,
        AVATAR_USER,
        AVATAR_ADD,
        AVATAR_CUSTOM
    };
    Q_ENUM(AvatarTypeEnum);

public:
    explicit SelectAvatarPage(QWidget *parent = nullptr);

    ~SelectAvatarPage();

    ///@brief 当前选中的头像路径
    QString currentSelectAvatar() const;

    ///@brief 记录跳转之前所在的页面
    void setMode(SelectAvatarMode mode);

    ///@brief 获取跳转之前所在页面
    SelectAvatarMode mode() const;

    ///@brief 设置当前头像，若不存在则新建并添加进显示
    void setCurrentAvatar(const QString &iconPath);

signals:

    ///@brief 当用户点击确认或取消时,发出该信号，mode标识着上个页面，isConfirm标识时确认还是保存
    void sigReturnToPrevPage(SelectAvatarMode mode, bool isConfirm);

private:
    ///@brief 初始化界面
    void initUI();

    ///@brief 加载系统头像
    void loadAvatar();

    ///@brief 往布局中添加头像
    ///         iconPath  头像路径
    ///         isChecked 是否被勾选
    ///         isSystem  是否是系统头像(非系统头像和系统头像ObjectName会不一样)
    AvatarButton *addAvatar(const QString &iconPath, AvatarTypeEnum type, bool isChecked);

    ///@brief 删除其他头像(非系统头像和用户添加按钮)
    void removeUserAvatar();

    ///@brief 移动添加自定义头像到最后
    void moveAddButtonToEnd();

private:
    QLayout *m_mainLayout;
    FlowLayout *m_flowLayout;
    QScrollArea *m_scrollArea;
    QLayout *m_btnLayout;
    QButtonGroup *m_buttonGroup;
    SelectAvatarMode m_mode;
    AvatarButton *m_addButton;
};

#endif  // SELECTAVATARPAGE_H
