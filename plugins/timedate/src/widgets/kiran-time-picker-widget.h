#ifndef KIRANTIMEPICKERWIDGET_H
#define KIRANTIMEPICKERWIDGET_H

#include <QWidget>
#include <QTime>

namespace Ui {
class KiranTimePickerWidget;
}

class KiranTimePickerWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor hightLightColor READ hightLightColor WRITE setHightLightColor)
public:
    enum TimePickerType{
        PICKER_HOUR_MINUTE,
        PICKER_HOUR_MINUTE_SECOND
    };
    explicit KiranTimePickerWidget(QWidget *parent = nullptr);
    explicit KiranTimePickerWidget(TimePickerType type,QWidget* parent = nullptr);
    ~KiranTimePickerWidget();

    void setPickerType(TimePickerType pickerType);

    QTime currentTime();
    void setCurrentTime(const QTime& time);

    QColor hightLightColor() const;

    void reset();
public slots:
    void setHightLightColor(QColor hightLightColor);

Q_SIGNALS:
    void currentTimeChanged(QTime currentTime);

private:
    void initUI();
    void emitCurrentTimeChanged();
protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    Ui::KiranTimePickerWidget *ui;
    TimePickerType m_pickerType;
    QColor m_hightLightColor;
};

#endif // KIRANTIMEPICKERWIDGET_H
