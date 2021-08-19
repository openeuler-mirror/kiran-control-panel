/**
 * @file touchpad-settings.h
 * @brief  初始化触摸板属性，并处理用户切换属性信号
 * @author yuanxing <yuanxing@kylinos.com.cn>
 * @copyright (c) 2020 KylinSec. All rights reserved.
 */

#ifndef TOUCHPAD_PAGE_H
#define TOUCHPAD_PAGE_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>

namespace Ui {
class TouchPadPage;
}

class KiranSwitchButton;
class KSMTouchPadProxy;
class TouchPadPage : public QWidget
{
    Q_OBJECT

public:
    explicit TouchPadPage(QWidget *parent = 0);
    ~TouchPadPage();

public:
    void initUI();

    virtual QSize sizeHint() const override;

private:
    void addComboBoxItem();
    void setDisableWidget(bool);
    void initComponent();

public slots:
    void onSliderValueChange();
    void onDisabelTouchPadToggled(bool disabled);

private:
    Ui::TouchPadPage *ui;
    QSharedPointer<KSMTouchPadProxy> m_touchPadInterface;
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
    QTimer *m_timer = nullptr;

};

#endif // TOUCHPAD_PAGE_H
