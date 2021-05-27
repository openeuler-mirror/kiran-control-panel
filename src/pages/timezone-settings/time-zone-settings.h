//
// Created by lxh on 2021/5/26.
//

#ifndef KIRAN_CPANEL_DATETIME_SRC_PAGES_TIMEZONE_SETTINGS_TIME_ZONE_SETTINGS_H_
#define KIRAN_CPANEL_DATETIME_SRC_PAGES_TIMEZONE_SETTINGS_TIME_ZONE_SETTINGS_H_

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class TimezoneSettings;
}
QT_END_NAMESPACE

class TimezoneSettings : public QWidget
{
    Q_OBJECT

public:
    explicit TimezoneSettings(QWidget *parent = nullptr);
    ~TimezoneSettings() override;

    void reset();

private:
    void init();

private slots:
    void handleSaveButtonClicked();
    void handleResetButtonClicked();

private:
    Ui::TimezoneSettings *ui;
};

#endif  //KIRAN_CPANEL_DATETIME_SRC_PAGES_TIMEZONE_SETTINGS_TIME_ZONE_SETTINGS_H_
