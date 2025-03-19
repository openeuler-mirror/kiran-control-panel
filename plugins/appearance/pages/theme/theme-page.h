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
class ThemePage;
}

class IconThemePage;
class SettingBriefWidget;
class CursorThemePage;
class WMThemePage;
class ExclusionGroup;
class ThemePage : public QWidget
{
    Q_OBJECT
public:
    explicit ThemePage(QWidget *parent = 0);
    ~ThemePage();
    QSize sizeHint() const override;

private:
    void init();
    bool initUITheme();
    bool initIconTheme();
    bool initCursorTheme();
    void initWindowTheme();
    void createThemeWidget();

public slots:
    void handleThemeChange(int type);
    void onAutoSwitchWindowThemeChanged(bool enable);
    void onCurrentUiThemeChanged();

signals:
    void clicked();

private:
    Ui::ThemePage *ui;

    ExclusionGroup* m_uiThemeExclusionGroup;

    IconThemePage *m_iconThemePage;
    SettingBriefWidget *m_chooseIconWidget;
    QString m_currIconTheme;

    CursorThemePage *m_cursorThemePage;
    SettingBriefWidget *m_chooseCursorWidget;
    QString m_currCursorThemes;

    WMThemePage* m_wmThemePage;
    SettingBriefWidget *m_chooseWMThemeWidget;
    QString m_currWMTheme;
    bool m_enableAutoSwitchWindowTheme = false;
};

#endif  // THEMES_H
