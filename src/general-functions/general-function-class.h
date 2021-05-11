/**
 * @file general-function-class.h
 * @brief  鼠标触摸板通用函数接口
 * @author yuanxing <yuanxing@kylinos.com.cn>
 * @copyright (c) 2020 KylinSec. All rights reserved.
 */

#ifndef GENERALFUNCTIONCLASS_H
#define GENERALFUNCTIONCLASS_H
#include <QSlider>
#include <QLabel>

#define SLIDER_MINIMUM  0
#define SLIDER_MAXIMUN  99
#define DBUS_MOUSE_NANE       "com.kylinsec.Kiran.SessionDaemon.Mouse"
#define DBUS_MOUSE_PATH       "/com/kylinsec/Kiran/SessionDaemon/Mouse"
#define DBUS_TOUCHPAD_NAME    "com.kylinsec.Kiran.SessionDaemon.TouchPad"
#define DBUS_TOUCHPAD_PATH    "/com/kylinsec/Kiran/SessionDaemon/TouchPad"

/**
 * 左侧侧边栏列表项
 */
enum Items
{
    ITEM_MOUSE,
    ITEM_TOUCHPAD,
    ITEM_LAST
};

/**
 * 鼠标、触摸板页面
 */
enum Pages
{
    PAGE_MOUSE,
    PAGE_TOUCHPAD,
    PAGE_LAST
};

/**
 * 鼠标或触摸板移动速度
 */
enum MOTION_ACCELERATION
{
    MOTION_SLOW = -1,
    MOTION_STANDARD,
    MOTION_FAST,
    MOTION_LAST
};

/**
 * 手持模式
 */
enum HandMode
{
    RIGHT_HAND, //false:0
    LEFT_HAND, //true:1
    HAND_LAST
};

/**
 * 触摸板点击模式
 */
enum CLickMode
{
    MODE_PRESS_AND_TAP,  //false:0
    MODE_TAP,            //true:1
    MODE_LAST
};

/**
 * 触摸板滚动方式
 */
enum ScrollMothod
{
    SCROLL_TWO_FINGER,
    SCROLL_EDGE,
    /*暂时不设置该模式
    SCROLL_BUTTON,*/
    SCROLL_LAST
};

class GeneralFunctionClass
{
public:
    GeneralFunctionClass();
    static int  convertValue(QSlider* ,QLabel*, bool,int);
};

#endif // GENERALFUNCTIONCLASS_H
