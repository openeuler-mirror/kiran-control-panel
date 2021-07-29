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
 
#ifndef SELECTAVATARPAGE_H
#define SELECTAVATARPAGE_H

#include <QWidget>

class QLayout;
class FlowLayout;
class QButtonGroup;
class QScrollArea;
class AvatarItemButton;

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
    AvatarItemButton *addAvatar(const QString &iconPath, AvatarTypeEnum type, bool isChecked);

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
    AvatarItemButton *m_addButton;
};

#endif  // SELECTAVATARPAGE_H
