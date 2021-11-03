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

#ifndef THEMES_H
#define THEMES_H

#include <QPushButton>
#include <QWidget>

namespace Ui
{
class Themes;
}

class ComKylinsecKiranSessionDaemonAppearanceInterface;
class QButtonGroup;
class IconThemes;
class ThemeWidgetGroup;
class ChooserWidget;
class CursorThemes;
class Themes : public QWidget
{
    Q_OBJECT

public:
    explicit Themes(QWidget *parent = 0);
    ~Themes();
    void setPage(int index);

private:
    bool initUI();
    bool initThemesUI();
    bool initIconThemesUI();
    bool initCursorThemesUI();
    bool getThemes(int themeType);
    int getJsonValueFromString(QString jsonString, QStringList *themeName, QStringList *themePath);
    void createThemeWidget();

signals:
    void clicked();

private:
    Ui::Themes *ui;
    ComKylinsecKiranSessionDaemonAppearanceInterface *m_appearanceInterface;

    IconThemes *m_iconThemes;
    ChooserWidget *m_chooseIconWidget;
    QString m_currIconThemes;

    CursorThemes *m_cursorThemes;
    ChooserWidget *m_chooseCursorWidget;
    QString m_currCursorThemes;

    //Gtk 主题
    QStringList m_allThemesName;
    QStringList m_allThemesPath;

    QString m_currentTheme;
    QStringList m_themesName;
    ThemeWidgetGroup *m_themeWidgetGroup;
};

#endif  // THEMES_H
