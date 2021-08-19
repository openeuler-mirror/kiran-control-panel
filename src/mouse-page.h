/**
 * @file mouse-settings.h
 * @brief  初始化鼠标属性，并处理用户切换属性信号
 * @author yuanxing <yuanxing@kylinos.com.cn>
 * @copyright (c) 2020 KylinSec. All rights reserved.
 */

#ifndef MOUSE_PAGE_H
#define MOUSE_PAGE_H

#include <QWidget>
#include <QComboBox>
#include <QListWidgetItem>
#include <QSharedPointer>

class KSMMouseProxy;
class KiranSwitchButton;
class KiranListItem;
class KiranSwitchButton;
namespace Ui {
class MousePage;
}

class MousePage : public QWidget
{
    Q_OBJECT

public:
    explicit MousePage(QWidget *parent = 0);
    ~MousePage();
    void initUI();
    virtual QSize sizeHint() const override;
public:
    void initComponent();

public slots:
    void onSliderValueChange();

private:
    Ui::MousePage *ui;
    QSharedPointer<KSMMouseProxy> m_mouseInterface;

    bool m_mouseLeftHand = false;
    bool m_mouseNaturalScroll = false;
    bool m_middleEmulationEnabled = false;
    double m_mouseMotionAcceleration = 0.0;

    //鼠标是否按下的标志位
    bool m_mousePressed = false;
    QTimer *m_timer = nullptr;
};

#endif // MOUSE_PAGE_H
