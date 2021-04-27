//
// Created by lxh on 2021/4/6.
//

#ifndef KIRAN_POWER_MANAGER_SRC_PAGES_POWER_SETTINGS_POWER_SETTINGS_PAGE_H_
#define KIRAN_POWER_MANAGER_SRC_PAGES_POWER_SETTINGS_POWER_SETTINGS_PAGE_H_

#include <QWidget>
#include "power.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class PowerSettingsPage;
}
QT_END_NAMESPACE

class PowerInterface;
class PowerSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit PowerSettingsPage(QWidget *parent = nullptr);
    ~PowerSettingsPage() override;

private:
    void init();
    void initUI();
    void initConnection();
    void load();
    QString getTimeDescription(int seconds);
    bool getCurrentIdleAction(IdleAction& idleAction);

private slots:
    void handleIdleTimeActionCurrentIdxChanged(int idx);
    void handleMonitorOffTimeoutCurrentIdxChanged(int idx);

private:
    Ui::PowerSettingsPage *ui;
    PowerInterface* m_powerInterface;
};

#endif  //KIRAN_POWER_MANAGER_SRC_PAGES_POWER_SETTINGS_POWER_SETTINGS_PAGE_H_
