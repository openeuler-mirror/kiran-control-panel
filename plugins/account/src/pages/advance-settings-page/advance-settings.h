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

#ifndef ADVANCESETTINGS_H
#define ADVANCESETTINGS_H

#include <kiranwidgets-qt5/kiran-titlebar-window.h>
#include <QWidget>

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
class KiranTips;
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
    virtual void closeEvent(QCloseEvent *event) override;

private:
    Ui::AdvanceSettings *ui;
    QString m_userName;
    bool m_isConfirm;
    KiranTips *m_editTip = nullptr;

    KiranSwitchButton *m_switchUserID;
    KiranSwitchButton *m_switchUserHome;
};

#endif  // ADVANCESETTINGS_H
