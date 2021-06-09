#ifndef KIRANDATETIMEWIDGET_H
#define KIRANDATETIMEWIDGET_H

#include <QWidget>
#include <QGSettings>
#include <kiran-system-daemon/timedate-i.h>

QT_BEGIN_NAMESPACE
namespace Ui { class KiranTimeDateWidget; }
QT_END_NAMESPACE

class MaskWidget;
class KiranSwitchButton;
class QListWidgetItem;

class TimezoneSettings;
class DisplayFormatSettings;
class DateTimeSettings;
class KiranTimeDateWidget : public QWidget
{
    Q_OBJECT

public:
    KiranTimeDateWidget(QWidget *parent = nullptr);
    ~KiranTimeDateWidget();

    QSize sizeHint() const override;
private:
    void initUI();
    void initTimeZoneSettingsPage();
    void initDateTimeSettingsPage();
    void initDisplayFormatSettingsPage();
    void updateTimeLabel();
    void updateTimeZoneLabel();
    void setMaskWidgetVisible(bool visible);

private slots:
    void handleSidebarSelectionChanged();
    void handleAutoSyncToggled(bool checked);
    void handleSystemNTPChanged(bool ntp);
    void handleSysntemCanNTPChanged(bool canNtp);
    void handleSystemTimeZoneChanged(QString timeZone);
    void handleSystemHourFormatChanged(TimedateHourFormat hourFormat);
    void handleSystemLongDisplayFormatChanged(int idx);
    void handleSystemSecondShowingChanged(bool enable);

protected:
    virtual void timerEvent(QTimerEvent* event) override;

private:
    Ui::KiranTimeDateWidget *ui;
    int m_updateTimer = -1;
    QString m_curTimeDateFormat;
    bool m_showSeconds;
    TimedateHourFormat m_hourFormat;
    MaskWidget *m_maskWidget = nullptr;
    KiranSwitchButton *m_autoSyncSwitch = nullptr;
    TimezoneSettings *m_zoneSettingsPage = nullptr;
    DisplayFormatSettings *m_formatSettingsPage = nullptr;
    DateTimeSettings *m_dateTimeSettingsPage = nullptr;
};

#endif // KIRANDATETIMEWIDGET_H
