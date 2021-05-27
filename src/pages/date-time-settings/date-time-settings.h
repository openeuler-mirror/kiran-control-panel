//
// Created by lxh on 2021/5/26.
//

#ifndef KIRAN_CPANEL_DATETIME_SRC_PAGES_DATE_TIME_SETTINGS_DATE_TIME_SETTINGS_H_
#define KIRAN_CPANEL_DATETIME_SRC_PAGES_DATE_TIME_SETTINGS_DATE_TIME_SETTINGS_H_

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class DateTimeSettings;
}
QT_END_NAMESPACE

class DateTimeSettings : public QWidget
{
    Q_OBJECT

public:
    explicit DateTimeSettings(QWidget *parent = nullptr);
    ~DateTimeSettings() override;

    void reset();

private:
    void init();

private slots:
    void handleButtonSaveClicked();
    void handleButtonResetClicked();

private:
    Ui::DateTimeSettings *ui;
};

#endif  //KIRAN_CPANEL_DATETIME_SRC_PAGES_DATE_TIME_SETTINGS_DATE_TIME_SETTINGS_H_
