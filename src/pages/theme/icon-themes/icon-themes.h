/**
 * Copyright (c) 2020 ~ 2021 KylinSec Co., Ltd.
 * kiran-cpanel-appearance is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     yuanxing <yuanxing@kylinos.com.cn>
 */

#ifndef ICONTHEMES_H
#define ICONTHEMES_H

#include <QWidget>

namespace Ui
{
class IconThemes;
}

class ThemeWidgetGroup;
class ComKylinsecKiranSessionDaemonAppearanceInterface;
class IconThemes : public QWidget
{
    Q_OBJECT

public:
    explicit IconThemes(QWidget *parent = 0);
    ~IconThemes();
    bool initUI();

private:
    bool getIconThemes(int themeType);
    int getJsonValueFromString(QString jsonString);
    void createIconWidgets();

signals:
    void sigSetIconTheme(bool);

private:
    Ui::IconThemes *ui;
    ComKylinsecKiranSessionDaemonAppearanceInterface *m_appearanceInterface;
    QStringList m_iconThemes;
    QStringList m_iconThemesPath;
    QString m_currentIconTheme;
    ThemeWidgetGroup *m_iconThemeWidgetGroup;
};

#endif  // ICONTHEMES_H
