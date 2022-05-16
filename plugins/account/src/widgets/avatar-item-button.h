 /**
  * @Copyright (C) 2020 ~ 2021 KylinSec Co., Ltd.
  *
  * Author:     liuxinhao <liuxinhao@kylinos.com.cn>
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program; If not, see <http: //www.gnu.org/licenses/>. 
  */
 
#ifndef AVATARITEMBUTTON_H
#define AVATARITEMBUTTON_H

#include <QAbstractButton>
#include <QObject>

class AvatarItemButton : public QAbstractButton
{
    Q_OBJECT

public:
    AvatarItemButton(QWidget *parent = nullptr);
    virtual ~AvatarItemButton();
    void setIcon(const QString &iconPath);
    QString iconPath();

protected:
    void setIcon(const QIcon &icon);
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *e) override;

private:
    QRect m_realRect;
    double m_radius;
    bool m_mouseEnter;
    bool m_hover;
    QString m_iconPath;
};

#endif  // AVATARITEMBUTTON_H
