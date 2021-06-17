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
#define SLIDER_MAXIMUN  100

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
 * 手持模式
 */
enum HandMode
{
    RIGHT_HAND, //false:0
    LEFT_HAND, //true:1
    HAND_LAST
};


#endif // GENERALFUNCTIONCLASS_H
