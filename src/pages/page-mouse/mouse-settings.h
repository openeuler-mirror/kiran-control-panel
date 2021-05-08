#ifndef MOUSESETTINGS_H
#define MOUSESETTINGS_H

#include <QWidget>
#include <QComboBox>
#include <QListWidgetItem>

class ComKylinsecKiranSessionDaemonMouseInterface;
class KiranSwitchButton;
class KiranListItem;
class KiranSwitchButton;
namespace Ui {
class MouseSettings;
}

class MouseSettings : public QWidget
{
    Q_OBJECT

public:
    explicit MouseSettings(QWidget *parent = 0);
    ~MouseSettings();

public:
    bool initUI();
    void initPageMouseUI();

public slots:
    void onSliderValueChange();

private:
    Ui::MouseSettings *ui;
    ComKylinsecKiranSessionDaemonMouseInterface *m_mouseInterface;

    bool m_mouseLeftHand = false;
    bool m_mouseNaturalScroll = false;
    bool m_middleEmulationEnabled = false;
    double m_mouseMotionAcceleration = 0.0;

    bool m_mousePressed = false;
};

#endif // MOUSESETTINGS_H
