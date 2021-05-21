#ifndef MOUSESETTINGS_H
#define MOUSESETTINGS_H

#include <QWidget>
/**
 * @file mouse-settings.h
 * @brief  初始化鼠标属性，并处理用户切换属性信号
 * @author yuanxing <yuanxing@kylinos.com.cn>
 * @copyright (c) 2020 KylinSec. All rights reserved.
 */

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
    explicit MouseSettings(ComKylinsecKiranSessionDaemonMouseInterface* mouseInterface, QWidget *parent = 0);
    ~MouseSettings();

public:
    void initUI();
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

    //鼠标是否按下的标志位
    bool m_mousePressed = false;
    QTimer *m_timer;
};

#endif // MOUSESETTINGS_H
