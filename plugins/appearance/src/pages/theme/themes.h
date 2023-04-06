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

#ifndef THEMES_H
#define THEMES_H

#include <QPushButton>
#include <QWidget>

namespace Ui
{
class Themes;
}

class IconThemes;
class ThemeWidgetGroup;
class SettingBriefWidget;
class CursorThemes;
class Themes : public QWidget
{
    Q_OBJECT
public:
    explicit Themes(QWidget *parent = 0);
    ~Themes();
    void setPage(int index);
    virtual QSize sizeHint() const override;

private:
    bool initUI();
    bool initThemesUI();
    bool initIconThemesUI();
    bool initCursorThemesUI();
    bool getThemes(int themeType);
    int getJsonValueFromString(QString jsonString, QStringList *themeName, QStringList *themePath);
    void createThemeWidget();

public slots:
    void handleThemeChange(int type);

signals:
    void clicked();

private:
    Ui::Themes *ui;

    IconThemes *m_iconThemes;
    SettingBriefWidget *m_chooseIconWidget;
    QString m_currIconThemes;

    CursorThemes *m_cursorThemes;
    SettingBriefWidget *m_chooseCursorWidget;
    QString m_currCursorThemes;

    //Gtk 主题
    QStringList m_allThemesName;
    QStringList m_allThemesPath;

    bool m_enableAutoSwitchWindowTheme = false;
    QString m_currentTheme;
    QStringList m_themesName;
    ThemeWidgetGroup *m_themeWidgetGroup;
};

#endif  // THEMES_H
