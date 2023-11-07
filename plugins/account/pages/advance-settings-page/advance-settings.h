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
    ~AdvanceSettings();
    static bool exec(const QString& userName,AdvanceSettingsInfo& info);
    virtual QSize sizeHint() const;

signals:
    // 窗口关闭信号
    void sigClose();

private:
    explicit AdvanceSettings(QWidget *parent);
    void initUI();
    void setInfo(QString userName,const AdvanceSettingsInfo &info);
    AdvanceSettingsInfo getInfo();
    bool isConfirm();

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
