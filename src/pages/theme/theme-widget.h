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

#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

#include <QLabel>
#include <QObject>
#include <QWidget>
class ThemeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ThemeWidget(QSize iconSize, QString currentIconTheme, QString iconThemeName,
                         QStringList iconStringList, QWidget* parent = 0);
    explicit ThemeWidget(QString name, QString currentTheme, QWidget* parent = 0);
    explicit ThemeWidget(QSize cursorSize, QString currentCursorTheme, QString cursorThemeName,
                         const QList<QPixmap>& cursorListMap, QWidget* parent = 0);

    //explicit ThemeWidget(QSize iSize, QString name, const QList<QPixmap> &listMap , QWidget *parent = 0);
    ~ThemeWidget();
    void setSelectStatus(bool, int themeType);
    void setTheme(QString);
    QString getTheme();

signals:
    void clicked();

protected:
    virtual bool eventFilter(QObject* watched, QEvent* event);

private:
    QLabel* m_iconSelectLabel;
    QWidget* m_iconImgWidget;
    QWidget* m_currIconTheme;

    QLabel* m_selectLabel;
    QWidget* m_themeImgWidget;

    QLabel* m_cursorSelectLabel;
    QWidget* m_cursorWidget;

    QString m_theme;
};

#endif  // THEMEWIDGET_H
