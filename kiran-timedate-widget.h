#ifndef KIRANDATETIMEWIDGET_H
#define KIRANDATETIMEWIDGET_H

#include <QWidget>
#include <QGSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class KiranTimeDateWidget; }
QT_END_NAMESPACE

class KiranTimeDateWidget : public QWidget
{
    Q_OBJECT

public:
    KiranTimeDateWidget(QWidget *parent = nullptr);
    ~KiranTimeDateWidget();
private:
    void initUI();
    void updateTimeLabel();
    void updateTimeZoneLabel();
    void updateFont();
protected:
    virtual void timerEvent(QTimerEvent* event) override;
private:
    Ui::KiranTimeDateWidget *ui;
    int m_updateTimer;
    QGSettings m_mateInterfaceSettings;
};

#endif // KIRANDATETIMEWIDGET_H
