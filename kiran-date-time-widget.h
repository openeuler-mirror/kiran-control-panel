#ifndef KIRANDATETIMEWIDGET_H
#define KIRANDATETIMEWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class KiranDateTimeWidget; }
QT_END_NAMESPACE

class KiranDateTimeWidget : public QWidget
{
    Q_OBJECT

public:
    KiranDateTimeWidget(QWidget *parent = nullptr);
    ~KiranDateTimeWidget();
private:
    void initUI();
    void updateTimeLabel();
    void updateTimeZoneLabel();
protected:
    virtual void timerEvent(QTimerEvent* event) override;
private:
    Ui::KiranDateTimeWidget *ui;
    int m_updateTimer;
};

#endif // KIRANDATETIMEWIDGET_H
