/**
 * @file hardware-information-widget.h
 * @brief 显示硬件信息控件
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#ifndef HARDWAREINFORMATIONWIDGET_H
#define HARDWAREINFORMATIONWIDGET_H

#include <QWidget>
#include <QStringList>
#include <QImage>

namespace Ui {
class HardwareInformationWidget;
}

class HardwareInformationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HardwareInformationWidget(QWidget *parent = 0);
    ~HardwareInformationWidget();

private:
    void initUI(void);
    void readHardwareInfo(int infoType);
    void getJsonValueFromString(QString jsonString);
    void showListInfo();
    void setCpuLogo(QString cpuModel);
    void scaledPixmap(QImage img);

private:
    Ui::HardwareInformationWidget *ui;
    QStringList diskList;
    QStringList graphicsList;
    QStringList ethsList;
};

#endif // HARDWAREINFORMATIONWIDGET_H
