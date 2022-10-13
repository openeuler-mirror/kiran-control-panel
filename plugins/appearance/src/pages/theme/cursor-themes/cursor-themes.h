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

#ifndef CURSORTHEMES_H
#define CURSORTHEMES_H

#include <QObject>
#include <QWidget>

class ThemeWidgetGroup;
class CursorThemes : public QWidget
{
    Q_OBJECT
public:
    explicit CursorThemes(QWidget *parent = 0);
    bool initUI();
    void updateCursorTheme(QString newCursorTheme);

private:
    bool getCursorThemes(int Type);
    int getJsonValueFromString(QString jsonString);
    QWidget *createCursorWidget();
    QImage convertToNomalImage(const QImage &cursorImage);

signals:
    void sigSetCursorTheme(bool, QString themeName);

private:
    QString m_currentCursorTheme;

    QStringList m_cursorThemesName;
    QStringList m_cursorThemesPath;
    ThemeWidgetGroup *m_themeWidgetGroup;
};

#endif  // CURSORTHEMES_H
