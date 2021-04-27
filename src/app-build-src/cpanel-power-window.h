//
// Created by lxh on 2021/4/6.
//

#ifndef KIRAN_POWER_MANAGER_SRC_APP_BUILD_SRC_CPANEL_POWER_WINDOW_H_
#define KIRAN_POWER_MANAGER_SRC_APP_BUILD_SRC_CPANEL_POWER_WINDOW_H_

#include <kiran-titlebar-window.h>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class CPanelPowerWindow;
}
QT_END_NAMESPACE

class CPanelPowerWindow : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit CPanelPowerWindow(QWidget *parent = nullptr);
    ~CPanelPowerWindow() override;

private:
    Ui::CPanelPowerWindow *ui;
};

#endif  //KIRAN_POWER_MANAGER_SRC_APP_BUILD_SRC_CPANEL_POWER_WINDOW_H_
