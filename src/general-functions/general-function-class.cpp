/**
 * @file general-function-class.cpp
 * @brief  鼠标触摸板通用函数接口
 * @author yuanxing <yuanxing@kylinos.com.cn>
 * @copyright (c) 2020 KylinSec. All rights reserved.
 */

#include "general-function-class.h"
GeneralFunctionClass::GeneralFunctionClass()
{

}

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
        label->setText(QObject::tr(SLOW));
        scrollSpeed = MOTION_SLOW;
    }
    else if(value >= lowMiddleNum && value <= middleNum)
    {
        if(!isMousePressed)
        {
            slider->setValue(middleNum);
        }
        label->setText(QObject::tr(STANDARD));
        scrollSpeed = MOTION_STANDARD;
    }
    else if(value >middleNum && value<= highMiddleNum)
    {
        if(!isMousePressed)
        {
            slider->setValue(middleNum);
        }
        label->setText(QObject::tr(STANDARD));
        scrollSpeed = MOTION_STANDARD;
    }
    else
    {
        if(!isMousePressed)
        {
            slider->setValue(SLIDER_MAXIMUN);
        }
        label->setText(QObject::tr(FAST));
        scrollSpeed = MOTION_FAST;
    }
    return scrollSpeed;
}
