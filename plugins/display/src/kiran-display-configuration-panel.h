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
 * Author:     yangxiaoqing <yangxiaoqing@kylinsec.com.cn>
 */

#ifndef KIRANDISPLAYCONFIGURATIONPANEL_H
#define KIRANDISPLAYCONFIGURATIONPANEL_H

#include <QFrame>
#include <QMap>

namespace Ui {
class KiranDisplayConfigurationPanel;
}

class KiranDisplayConfigurationPanel : public QFrame
{
    Q_OBJECT

public:
    explicit KiranDisplayConfigurationPanel(QWidget *parent = 0);
    ~KiranDisplayConfigurationPanel();

    QVariantMap getData();
    void setData(const QVariantList &list, const bool &clearChecked=false);
    void changeItemDisabled(const bool &disabled);

protected:
    void paintEvent(QPaintEvent *event) override;

Q_SIGNALS:
    void buttonChecked(QString monitorPath);

private slots:
    //void paintEvent(QPaintEvent *event);
    void on_pushButton_left_clicked();
    void on_pushButton_horizontal_clicked(bool checked);
    void on_pushButton_vertical_clicked(bool checked);
    void on_pushButton_right_clicked();
    void on_pushButton_identifying_clicked();

private:
    Ui::KiranDisplayConfigurationPanel *ui;
   //QVariantList m_textList;
   // QMap<QString, QSize> m_textSize;
};

#endif // KIRANDISPLAYCONFIGURATIONPANEL_H
