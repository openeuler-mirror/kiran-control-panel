//
// Created by lxh on 2021/4/6.
//

#ifndef KIRAN_POWER_MANAGER_SRC_APP_BUILD_SRC_KIRAN_POWER_MANAGER_WINDOW_H_
#define KIRAN_POWER_MANAGER_SRC_APP_BUILD_SRC_KIRAN_POWER_MANAGER_WINDOW_H_

#include <QWidget>
#include <kiran-titlebar-window.h>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class KiranPowerManagerWindow;
}
QT_END_NAMESPACE

class KiranPowerManagerWindow : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    explicit KiranPowerManagerWindow(QWidget *parent = nullptr);
    ~KiranPowerManagerWindow() override;

private:
    Ui::KiranPowerManagerWindow *ui;
};

#endif  //KIRAN_POWER_MANAGER_SRC_APP_BUILD_SRC_KIRAN_POWER_MANAGER_WINDOW_H_
