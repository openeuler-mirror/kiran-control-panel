/**
 * @file kiran-system-information.h
 * @brief 显示系统信息页
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#ifndef KIRANSYSTEMINFORMATION_H
#define KIRANSYSTEMINFORMATION_H

#include <QWidget>
#include "information-list-item.h"
#include "hardware-information-widget.h"

namespace Ui {
class kiranSystemInformation;
}

class kiranSystemInformation : public QWidget
{
    Q_OBJECT

public:
    explicit kiranSystemInformation(QWidget *parent = 0);
    ~kiranSystemInformation();

private:
    void initUI();
    InformationListItem *createInformationItem(const QString text , const QString iconPath);
    void addPagesToStackWidget(void);
    QWidget* createSystemInfoPage(void);
    QWidget* createHardwareInfoPage(void);


signals:

public slots:

protected:
    //void paintEvent(QPaintEvent*);

private:
    enum {pageSystemInfo, PageHardwareInfo};
    Ui::kiranSystemInformation *ui;
    InformationListItem* systemInfomationItem;
    InformationListItem* hardwareInformationItem;

    QWidget* SystemInformation;
    QWidget* hardwareInformation;


};

#endif // KIRANSYSTEMINFORMATION_H
