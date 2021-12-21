//
// Created by lxh on 2021/4/6.
//

#ifndef KIRAN_POWER_MANAGER_SRC_PAGES_GENERAL_SETTINGS_GENERAL_SETTINGS_PAGE_H_
#define KIRAN_POWER_MANAGER_SRC_PAGES_GENERAL_SETTINGS_GENERAL_SETTINGS_PAGE_H_

#include <QTimer>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class GeneralSettingsPage;
}
QT_END_NAMESPACE

class PowerInterface;
class KiranSwitchButton;
class QGSettings;
class GeneralSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralSettingsPage(QWidget *parent = nullptr);
    ~GeneralSettingsPage() override;
    virtual QSize sizeHint() const override;

private:
    void init();
    void initSessionSetting();
    void initUI();
    void initConnection();

    void load();

    void setBrightnessPercent(int percent);
    void updateIdleTimeLabel(int min);

private slots:
    void handleComboBoxCurrentIdxChanged(int idx);

private:
    Ui::GeneralSettingsPage *ui;
    PowerInterface* m_powerInterface;
    QTimer m_brightnessTimer;
    QTimer m_idleTimeTimer;
    int m_brightnessValue;
    QGSettings* m_sessionSettings = nullptr;
    QGSettings* m_screensaverSettings = nullptr;
    KiranSwitchButton* m_btn_lockScreen = nullptr;
};

#endif  //KIRAN_POWER_MANAGER_SRC_PAGES_GENERAL_SETTINGS_GENERAL_SETTINGS_PAGE_H_
