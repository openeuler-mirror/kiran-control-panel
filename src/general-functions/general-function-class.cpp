/**
 * @file general-function-class.cpp
 * @brief  鼠标触摸板通用函数接口
 * @author yuanxing <yuanxing@kylinos.com.cn>
 * @copyright (c) 2020 KylinSec. All rights reserved.
 */

#include <QObject>
#include "general-function-class.h"
GeneralFunctionClass::GeneralFunctionClass()
{

}

/**
 * @brief 根据滑动条当前值的范围，转化为定义的三 个等级：低速、标准、快速
 * @param[in] slider 引起值变化的滑动条
 * @param[in] label 显示速度等级的标签
 * @param[in] isMousePressed 鼠标是否按下的标志位
 * @param[in] value 滑动条当前值
 * @return 转化后的速度等级
 */
int GeneralFunctionClass::convertValue(QSlider *slider , QLabel *label, bool isMousePressed,int value)
{
    int lowMiddleNum = (SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/4;
    int middleNum = (SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/2;
    int highMiddleNum = (SLIDER_MAXIMUN-SLIDER_MINIMUM+1)/4*3;

    int scrollSpeed;

    if(value >= SLIDER_MINIMUM && value < lowMiddleNum)
    {
        if(!isMousePressed)
        {
            slider->setValue(SLIDER_MINIMUM);
        }
        label->setText(QObject::tr("SLow"));
        scrollSpeed = MOTION_SLOW;
    }
    else if(value >= lowMiddleNum && value <= middleNum)
    {
        if(!isMousePressed)
        {
            slider->setValue(middleNum);
        }
        label->setText(QObject::tr("Standard"));
        scrollSpeed = MOTION_STANDARD;
    }
    else if(value >middleNum && value<= highMiddleNum)
    {
        if(!isMousePressed)
        {
            slider->setValue(middleNum);
        }
        label->setText(QObject::tr("Standard"));
        scrollSpeed = MOTION_STANDARD;
    }
    else
    {
        if(!isMousePressed)
        {
            slider->setValue(SLIDER_MAXIMUN);
        }
        label->setText(QObject::tr("Fast"));
        scrollSpeed = MOTION_FAST;
    }
    return scrollSpeed;
}
