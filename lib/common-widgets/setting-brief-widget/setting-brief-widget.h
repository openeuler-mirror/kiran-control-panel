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
 * Author:     yuanxing <yuanxing@kylinsec.com.cn>
 */

#ifndef CHOOSERWIDGET_H
#define CHOOSERWIDGET_H

#include <QWidget>

namespace Ui
{
class SettingBriefWidget;
}

/**
 * 用于提供个性化设置的设置行，显示当前配置项值。也提供点击跳转的功能
 */
class SettingBriefWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SettingBriefWidget(QString title, int type = -1, QWidget *parent = 0);
    ~SettingBriefWidget();

    void setName(QString name);

private:
    void initUI(QString title);
    QString getThemeArrowIcon();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);

signals:
    void clicked();

private slots:
    void updateThemeArrowIcon();

private:
    Ui::SettingBriefWidget *ui;
    int m_wallpaperType = -1;
};

#endif  // CHOOSERWIDGET_H
