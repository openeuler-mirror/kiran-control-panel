#ifndef KIRANDATETIMEWIDGET_H
#define KIRANDATETIMEWIDGET_H

#include <QWidget>
#include <QGSettings>
#include <kiranwidgets-qt5/kiran-titlebar-window.h>

QT_BEGIN_NAMESPACE
namespace Ui { class KiranTimeDateWidget; }
QT_END_NAMESPACE

class KiranTimeDateWidget : public KiranTitlebarWindow
{
    Q_OBJECT

public:
    KiranTimeDateWidget(QWidget *parent = nullptr);
    ~KiranTimeDateWidget();
private:
    void initUI();
    void updateTimeLabel();
    void updateTimeZoneLabel();
protected:
    virtual void timerEvent(QTimerEvent* event) override;
private:
    Ui::KiranTimeDateWidget *ui;
    int m_updateTimer;
};

#endif // KIRANDATETIMEWIDGET_H
