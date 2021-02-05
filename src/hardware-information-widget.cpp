/**
 * @file hardware-information-widget.cpp
 * @brief 显示硬件信息控件
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#include "hardware-information-widget.h"
#include "ui_hardware-information-widget.h"

HardwareInformationWidget::HardwareInformationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HardwareInformationWidget)
{
    ui->setupUi(this);
    initUI();
}

HardwareInformationWidget::~HardwareInformationWidget()
{
    delete ui;
}

void HardwareInformationWidget::initUI(void)
{

}
