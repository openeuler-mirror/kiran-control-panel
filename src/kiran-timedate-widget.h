#ifndef KIRANDATETIMEWIDGET_H
#define KIRANDATETIMEWIDGET_H

#include <QWidget>
#include <QGSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class KiranTimeDateWidget; }
QT_END_NAMESPACE

class MaskWidget;
class KiranSwitchButton;
class QListWidgetItem;
class KiranTimeDateWidget : public QWidget
{
    Q_OBJECT

public:
    KiranTimeDateWidget(QWidget *parent = nullptr);
    ~KiranTimeDateWidget();

private:
    void initUI();
    void loadStyleSheet();
    void updateTimeLabel();
    void updateTimeZoneLabel();
    void setMaskWidgetVisible(bool visible);
protected:
    virtual void timerEvent(QTimerEvent* event) override;

private:
    Ui::KiranTimeDateWidget *ui;
    int m_updateTimer = -1;
    MaskWidget *m_maskWidget = nullptr;
    KiranSwitchButton *m_autoSyncSwitch = nullptr;
    QListWidgetItem *m_changeTimeZoneItem = nullptr;
    QListWidgetItem *m_setTimeManuallyItem = nullptr;
};

#endif // KIRANDATETIMEWIDGET_H
