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
    void    clear()
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
    explicit AdvanceSettings();
    ~AdvanceSettings();

    /**
     * @brief 更新信息进入高级设置窗口
     * @param 当前创建的账户
     * @param 创建账户中保存的用户高级设置信息
     */
    void setInfo(QString account, AdvanceSettingsInfo &info);

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
    QString              m_account;
    bool                 m_isConfirm;
    KiranTips *          m_editTip = nullptr;
    KiranSwitchButton *  m_specifyUserIDSwitch;
    KiranSwitchButton *  m_specifyUserHomeSwitch;
};

#endif  // ADVANCESETTINGS_H
