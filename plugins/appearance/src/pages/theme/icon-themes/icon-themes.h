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

#ifndef ICONTHEMES_H
#define ICONTHEMES_H

#include <QWidget>

namespace Ui
{
class IconThemes;
}

class ThemeWidgetGroup;
class IconThemes : public QWidget
{
    Q_OBJECT

public:
    explicit IconThemes(QWidget *parent = 0);
    ~IconThemes();
    bool initUI();
    void updateIconTheme(QString newIconTheme);

private:
    bool getIconThemes();
    int getJsonValueFromString(QString jsonString);
    void createIconWidgets();

signals:
    void sigSetIconTheme(bool, QString);

private:
    Ui::IconThemes *ui;
    QStringList m_iconThemes;
    QStringList m_iconThemesPath;
    QString m_currentIconTheme;
    ThemeWidgetGroup *m_iconThemeWidgetGroup;
};

#endif  // ICONTHEMES_H
