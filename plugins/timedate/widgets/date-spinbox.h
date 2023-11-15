/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-control-panel is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */

#ifndef YEARSPINBOX_H
#define YEARSPINBOX_H

#include <QSpinBox>
#include <QDate>

#include <QDebug>

class YearSpinBox:public QSpinBox
{
Q_OBJECT
public:
    explicit YearSpinBox(QWidget* parent=nullptr):QSpinBox(parent){
        setAccessibleName("SpinBoxYear");
    }
    ~YearSpinBox(){}
protected:
    virtual QString textFromValue(int val) const Q_DECL_OVERRIDE{
        QDate date;
        date.setDate(val,1,1);
        return date.toString(tr("yyyy"));
    }
};

class MonthSpinBox:public QSpinBox
{
Q_OBJECT
public:
    explicit MonthSpinBox(QWidget* parent=nullptr):QSpinBox(parent){
        setAccessibleName("SpinBoxMonth");
    }
    ~MonthSpinBox(){}
protected:
    virtual QString textFromValue(int val) const Q_DECL_OVERRIDE{
        QDate date;
        date.setDate(1970,val,1);
        return date.toString(tr("MMMM"));
    }
};

class DaySpinBox:public QSpinBox
{
Q_OBJECT
public:
    explicit DaySpinBox(QWidget* parent=nullptr):QSpinBox(parent){
        setAccessibleName("SpinBoxDay");
    }
    ~DaySpinBox(){}
protected:
    virtual QString textFromValue(int val) const Q_DECL_OVERRIDE{
        QString dayString = QString(tr("%1")).arg(val,2,10,QChar('0'));
        return dayString;
    }
};

#endif // YEARSPINBOX_H
