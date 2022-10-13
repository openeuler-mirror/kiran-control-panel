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
    void updateScrollPickerColor();

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
