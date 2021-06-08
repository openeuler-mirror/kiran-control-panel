/**
 * @file touchpad-settings.h
 * @brief  初始化触摸板属性，并处理用户切换属性信号
 * @author yuanxing <yuanxing@kylinos.com.cn>
 * @copyright (c) 2020 KylinSec. All rights reserved.
 */

#ifndef TOUCHPADSETTINGS_H
#define TOUCHPADSETTINGS_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>

namespace Ui {
class TouchPadSettings;
}

class KiranSwitchButton;
class ComKylinsecKiranSessionDaemonTouchPadInterface;
class TouchPadSettings : public QWidget
{
    Q_OBJECT

public:
    explicit TouchPadSettings(QWidget *parent = 0);
    ~TouchPadSettings();

public:
    void initUI();
    void initPageTouchPadUI();

private:
    void addComboBoxItem();
    void setDisableWidget(bool);

public slots:
    void onSliderValueChange();
    void onDisabelTouchPadToggled(bool disabled);

private:
    Ui::TouchPadSettings *ui;
    ComKylinsecKiranSessionDaemonTouchPadInterface *m_touchPadInterface;
    QList<QComboBox* > m_comboBoxList;
    QList<KiranSwitchButton*> m_checkBoxList;
    QList<QLabel*> m_labelList;

    bool m_disabelWhileTyping = true;
    bool m_touchPadLeftHand = false;
    bool m_touchPadNaturalScroll = false;
    bool m_tapToClick = true;
    bool m_touchPadEnabled = true;
    double m_touchPadMotionAcceleration = 0.0;
    int m_clickMethod = 0;
    int m_scrollMethod = 0;

    //鼠标是否按下的标志位
    bool m_mousePressed = false;
    QTimer *m_timer;

};

#endif // TOUCHPADSETTINGS_H
