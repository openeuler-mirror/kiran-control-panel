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
 
#ifndef ADVANCESETTINGS_H
#define ADVANCESETTINGS_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <QWidget>

#include "kiran-tips.h"

namespace Ui
{
class AdvanceSettings;
}

struct AdvanceSettingsInfo
{
    QString shell;
    QString uid;
    QString homeDir;
    void clear()
    {
        shell.clear();
        uid.clear();
        homeDir.clear();
    }
};
class KiranSwitchButton;
class AdvanceSettings : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit AdvanceSettings(QWidget *parent);
    ~AdvanceSettings();

    /**
     * @brief 更新信息进入高级设置窗口
     * @param 当前创建的用户
     * @param 创建用户中保存的用户高级设置信息
     */
    void setInfo(QString userName, AdvanceSettingsInfo &info);

    /**
     * @brief 从高级设置窗口取出确认的信息
     * @return 用户高级设置信息
     */
    AdvanceSettingsInfo getInfo();

    /**
     * @brief 是否是保存退出
     * @return
     */
    bool isConfirm();

    virtual QSize sizeHint() const;

signals:
    /**
     * @brief 窗口关闭信号
     */
    void sigClose();

private:
    void initUI();

protected:
    virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::AdvanceSettings *ui;
    QString m_userName;
    bool m_isConfirm;
    KiranTips *m_editTip = nullptr;
    KiranSwitchButton *m_specifyUserIDSwitch;
    KiranSwitchButton *m_specifyUserHomeSwitch;
};

#endif  // ADVANCESETTINGS_H
