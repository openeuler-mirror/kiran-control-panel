//
// Created by lxh on 2021/5/26.
//

#ifndef KIRAN_CPANEL_DATETIME_SRC_PAGES_DISPLAY_FORMAT_DISPLAY_FORMAT_SETTINGS_H_
#define KIRAN_CPANEL_DATETIME_SRC_PAGES_DISPLAY_FORMAT_DISPLAY_FORMAT_SETTINGS_H_

#include <QWidget>
#include <kiran-system-daemon/timedate_i.h>

QT_BEGIN_NAMESPACE
namespace Ui
{
class DisplayFormatSettings;
}
QT_END_NAMESPACE

class KiranSwitchButton;
class DisplayFormatSettings : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayFormatSettings(QWidget *parent = nullptr);
    ~DisplayFormatSettings() override;

private:
    void init();

private slots:
    void handleLongDisplayFormatChanged(int idx);
    void handleSystemLongDisplayFormatChanged(int idx);

    void handleShortgDisplayFormatChanged(int idx);
    void handleSystemShortDisplayFormatChanged(int idx);

    void handleHourFormatChanged(int idx);
    void handleSystemHourFormatChanged(TimedateHourFormat format);

    void handleTimeSecondVisibleChanged(bool enable);
    void handleSystemTimeSecondVisibleChanged(bool enable);

private:
    Ui::DisplayFormatSettings *ui;
    KiranSwitchButton *m_showSecondSwitch= nullptr;
};

#endif  //KIRAN_CPANEL_DATETIME_SRC_PAGES_DISPLAY_FORMAT_DISPLAY_FORMAT_SETTINGS_H_
