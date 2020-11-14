#ifndef YEARSPINBOX_H
#define YEARSPINBOX_H

#include <QSpinBox>
#include <QDate>

#include <QDebug>

class YearSpinBox:public QSpinBox
{
Q_OBJECT
public:
    explicit YearSpinBox(QWidget* parent=nullptr):QSpinBox(parent){}
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
    explicit MonthSpinBox(QWidget* parent=nullptr):QSpinBox(parent){}
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
    explicit DaySpinBox(QWidget* parent=nullptr):QSpinBox(parent){}
    ~DaySpinBox(){}
protected:
    virtual QString textFromValue(int val) const Q_DECL_OVERRIDE{
        QString dayString = QString(tr("%1")).arg(val,2,10,QChar('0'));
        return dayString;
    }
};

#endif // YEARSPINBOX_H
