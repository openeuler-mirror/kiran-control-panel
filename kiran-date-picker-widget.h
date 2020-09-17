#ifndef KIRANDATEPICKERWIDGET_H
#define KIRANDATEPICKERWIDGET_H

#include <QWidget>
#include <QDate>

namespace Ui {
class KiranDatePickerWidget;
}

class KiranDatePickerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KiranDatePickerWidget(QWidget *parent = nullptr);
    ~KiranDatePickerWidget();

    QDate currentDate();
    void setCurrentDate(QDate date);

    void reset();
private:
    void initUI();
private:
    Ui::KiranDatePickerWidget *ui;
};

#endif // KIRANDATEPICKERWIDGET_H
