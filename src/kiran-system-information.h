/**
 * @file kiran-system-information.h
 * @brief 显示系统信息页
 * @author yuanxing@kylinos.com.cn
 * @copyright Copyright ©2020 KylinSec. All rights reserved.
 */

#ifndef KIRANSYSTEMINFORMATION_H
#define KIRANSYSTEMINFORMATION_H

#include <QWidget>
#include <QListWidgetItem>
#include <QStackedWidget>
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
    QWidget* createSystemInfoPage(void);
    QWidget* createHardwareInfoPage(void);
    virtual void resizeEvent(QResizeEvent *event) override;


signals:
    void sigClose();

public slots:
    void changeWidgetWhenItemClicked(QListWidgetItem* currentItem);

protected:
    //void paintEvent(QPaintEvent*);

private:
    enum {pageSystemInfo, PageHardwareInfo};
    enum {itemSystemInfo, itemHardwareInfo};
    Ui::kiranSystemInformation *ui;
    QStackedWidget *stackedWidget;

    InformationListItem* systemInfomationItem;
    InformationListItem* hardwareInformationItem;

    QWidget* systemInfoWidget;
    QWidget* hardwareInfoWidget;


};

#endif // KIRANSYSTEMINFORMATION_H
