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
class GeneralSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralSettingsPage(QWidget *parent = nullptr);
    ~GeneralSettingsPage() override;

private:
    void init();
    void initUI();
    void initConnection();
    void load();
    void setBrightnessPercent(quint16 percent);
    void setBrightnessPercentLabel(quint16 percent);

private slots:
    void handleComboBoxCurrentIdxChanged(int idx);

private:
    Ui::GeneralSettingsPage *ui;
    PowerInterface* m_powerInterface;
    QTimer m_brightnessTimer;
    int m_brightnessValue;
};

#endif  //KIRAN_POWER_MANAGER_SRC_PAGES_GENERAL_SETTINGS_GENERAL_SETTINGS_PAGE_H_
