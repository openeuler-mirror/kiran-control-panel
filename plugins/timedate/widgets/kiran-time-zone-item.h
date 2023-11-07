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
#ifndef KIRANTIMEZONEITEM_H
#define KIRANTIMEZONEITEM_H

#include <QWidget>

#include "dbus-api-wrapper/timedate-interface.h"

namespace Ui {
class KiranTimeZoneItem;
}

class KiranTimeZoneItem : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor searchHighLightColor READ searchHighLightColor WRITE setSearchHighLightColor)
    Q_PROPERTY(bool isSelected READ isSelected WRITE setisSelected NOTIFY isSelectedChanged)
public:
    explicit KiranTimeZoneItem(const ZoneInfo zoneInfo,QWidget *parent = nullptr);
    explicit KiranTimeZoneItem(QWidget* parent=nullptr);

    ~KiranTimeZoneItem();

    QString getTimeZoneID();

    QColor searchHighLightColor() const;

    bool isSelected() const;

    void setHeightLightKeyword(const QString& keyword);
    void unsetHeightLightKeyword();
Q_SIGNALS:
    void clicked();

    void isSelectedChanged(bool isSelected);

public slots:
    void setSearchHighLightColor(QColor searchHighLightColor);
    void seletedZoneInfoChanged(const QString& zoneID);
    void setisSelected(bool isSelected);
    void mousePressEvent(QMouseEvent *event) override;

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    Ui::KiranTimeZoneItem *ui;
    QPixmap m_selectedIcon;
    QColor m_searchHighLightColor;
    bool m_isSelected;
    QString m_keyword;
    QString m_displayUTC;
    QString m_city;
    QString m_zoneID;
    bool m_isResultTip;
};

#endif // KIRANTIMEZONEITEM_H
